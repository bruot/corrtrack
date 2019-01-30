/*
 * This file is part of the particle tracking software CorrTrack.
 *
 * Copyright 2016, 2017 Nicolas Bruot
 *
 *
 * CorrTrack is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * CorrTrack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with CorrTrack.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <QString>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QToolButton>
#include <QSpacerItem>
#include <QWidget>
#include <QIntValidator>
#include <QMessageBox>
#include "constants.h"
#include "corrtrackwindow.h"
#include "math/math.h"
#include "movie/movie.h"
#include "movieintensityminmaxworker.h"
#include "intensitydialog.h"


using IntensityMode = CorrTrackWindow::IntensityMode;


IntensityDialog::IntensityDialog(IntensityMode intensityMode,
                                 const uint16_t intensityMin,
                                 const uint16_t intensityMax,
                                 Movie* movie, size_t frameIndex,
                                 QWidget* parent)
    : OKCancelDialog(parent),
      frameIndex{frameIndex},
      movie{movie},
      minMaxRB{new QRadioButton("Set intensity boundaries", this)},
      autoVarRB{new QRadioButton("Auto frame-variable min-max", this)},
      minMaxWidget{new QWidget(this)},
      minLE{new QLineEdit(QString::number(intensityMin), this)},
      maxLE{new QLineEdit(QString::number(intensityMax), this)},
      bitDepthLE{new QLineEdit(this)},
      bitDepthBtn{new QToolButton(this)},
      autoFrameBtn{new QToolButton(this)},
      autoMovieBtn{new QToolButton(this)},
      taskThread{nullptr},
      movieIntensityMinMaxWorker{nullptr},
      progressWindow{nullptr},
      movieMin{0},
      movieMax{255}
{
    setWindowTitle("Image intensity");

    const int lineEditWidth = 50;

    QIntValidator *bitDepthValidator = new QIntValidator(constants::BIT_DEPTH_MIN_VALUE,
                                                         constants::BIT_DEPTH_MAX_VALUE,
                                                         this);
    QIntValidator *intensityValidator = new QIntValidator(0,
                                                          constants::INTENSITY_MAX_VALUE,
                                                          this);
    bitDepthLE->setText(QString::number(movie->bitDepth));
    bitDepthLE->setValidator(bitDepthValidator);
    minLE->setValidator(intensityValidator);
    maxLE->setValidator(intensityValidator);

    QHBoxLayout* minMaxLELayout = new QHBoxLayout();
    minLE->setFixedWidth(lineEditWidth);
    maxLE->setFixedWidth(lineEditWidth);
    minMaxLELayout->addItem(new QSpacerItem(constants::NESTED_LISTS_TAB_SPACING, 0));
    QString labelText;
    labelText = QString("Min (0-")
            + QString::number(constants::INTENSITY_MAX_VALUE)
            + QString(")");
    minMaxLELayout->addWidget(new QLabel(labelText, this));
    minMaxLELayout->addWidget(minLE);
    labelText = QString("Max (0-")
            + QString::number(constants::INTENSITY_MAX_VALUE)
            + QString(")");
    minMaxLELayout->addWidget(new QLabel(labelText, this));
    minMaxLELayout->addWidget(maxLE);
    minMaxLELayout->addStretch();
    //
    QHBoxLayout* bitDepthLayout = new QHBoxLayout();
    bitDepthLayout->addItem(new QSpacerItem(2 * constants::NESTED_LISTS_TAB_SPACING, 0));
    if (isMinMaxBitDepth())
        bitDepthLE->setText(QString::number(this->getBitDepth()));
    bitDepthLE->setFixedWidth(24);
    bitDepthLayout->addWidget(new QLabel("From bit depth:", this));
    bitDepthLayout->addWidget(bitDepthLE);
    bitDepthBtn->setText("Set");
    bitDepthLayout->addWidget(bitDepthBtn);
    bitDepthLayout->addStretch();
    //
    QHBoxLayout* autoLayout = new QHBoxLayout();
    autoLayout->addItem(new QSpacerItem(2 * constants::NESTED_LISTS_TAB_SPACING, 1));
    autoLayout->addWidget(new QLabel("Auto from:", this));
    autoFrameBtn->setText("Frame");
    autoLayout->addWidget(autoFrameBtn);
    autoMovieBtn->setText("Movie (may be slow)");
    autoLayout->addWidget(autoMovieBtn);
    autoLayout->addStretch();
    //
    QVBoxLayout *minMaxLayout = new QVBoxLayout(minMaxWidget);
    minMaxLayout->addLayout(minMaxLELayout);
    minMaxLayout->addLayout(bitDepthLayout);
    minMaxLayout->addLayout(autoLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(minMaxRB);
    mainLayout->addWidget(minMaxWidget);
    mainLayout->addWidget(autoVarRB);
    setLayout(mainLayout);
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    switch (intensityMode)
    {
    case IntensityMode::BitDepth:
    case IntensityMode::MinMax:
        minMaxRB->setChecked(true);
        break;
    case IntensityMode::AutoVariable:
        autoVarRB->setChecked(true);
        break;
    }

    updateIntensityWidgets();

    // These two lines connect the button push to IntensityDialog::ok() instead
    // of OKCancelDialog::ok().
    disconnect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));

    connect(minMaxRB, SIGNAL(clicked()), this, SLOT(updateIntensityWidgets()));
    connect(autoVarRB, SIGNAL(clicked()), this, SLOT(updateIntensityWidgets()));
    connect(bitDepthBtn, SIGNAL(clicked()), this, SLOT(setBitDepthMinMax()));
    connect(autoFrameBtn, SIGNAL(clicked()), this, SLOT(setFrameMinMax()));
    connect(autoMovieBtn, SIGNAL(clicked()), this, SLOT(setMovieMinMax()));
}

bool IntensityDialog::isMinMaxBitDepth() const
{
    // Checks if min is zero and (max + 1) is a power of two.
    unsigned long min = getIntensityMin();
    unsigned long max = getIntensityMax();
    return (min == 0) && (!((max + 1) & max));
}

IntensityMode IntensityDialog::getIntensityMode() const
{
    if (minMaxRB->isChecked())
    {
        // When using min and max, we split in two cases: when the min and max
        // values simplify to a known bit depth, and when they don't. This
        // allows using faster computations in the case of a known bit depth.
        if (isMinMaxBitDepth())
            return IntensityMode::BitDepth;
        else
            return IntensityMode::MinMax;
    }
    else
        return IntensityMode::AutoVariable;
}

unsigned int IntensityDialog::getBitDepth() const
{
    // This function is only meaningful in BitDepth mode.
    return math::ilog2(getIntensityMax() + 1);
}

unsigned long IntensityDialog::getIntensityMin() const
{
    return minLE->text().toULong();
}

unsigned long IntensityDialog::getIntensityMax() const
{
    return maxLE->text().toULong();
}

void IntensityDialog::updateIntensityWidgets()
{
    minMaxWidget->setEnabled(minMaxRB->isChecked());
}

void IntensityDialog::ok()
{
    // Validate single fields

    if (minMaxRB->isChecked())
    {
        QMessageBox *msgBox = new QMessageBox(this);
        int pos;

        pos = minLE->cursorPosition();
        if (minLE->validator()->validate(minLE->text(), pos) != QValidator::Acceptable)
        {
            msgBox->setText(QString("Min intensity outside acceptable range (0-%1).").arg(constants::INTENSITY_MAX_VALUE));
            msgBox->exec();
            return;
        }

        pos = maxLE->cursorPosition();
        if (maxLE->validator()->validate(maxLE->text(), pos) != QValidator::Acceptable)
        {
            msgBox->setText(QString("Max intensity outside acceptable range (0-%1).").arg(constants::INTENSITY_MAX_VALUE));
            msgBox->exec();
            return;
        }

        pos = bitDepthLE->cursorPosition();
        if (bitDepthLE->validator()->validate(bitDepthLE->text(), pos) != QValidator::Acceptable)
        {
            msgBox->setText(QString("Bit depth outside acceptable range (%1-%2).").arg(constants::BIT_DEPTH_MIN_VALUE).arg(constants::BIT_DEPTH_MAX_VALUE));
            msgBox->exec();
            return;
        }
    }

    // Validate fields combinations

    if (minMaxRB->isChecked() && (getIntensityMin() >= getIntensityMax()))
    {
        QMessageBox msgBox;
        msgBox.setText("The maximum intensity should be strictly lower than the minimum intensity.");
        msgBox.exec();
        return;
    }

    return OKCancelDialog::ok();
}

void IntensityDialog::setBitDepthMinMax()
{
    int pos = 0;
    QString text(bitDepthLE->text());
    if (bitDepthLE->validator()->validate(text, pos)
            != QValidator::Acceptable)
    {
        QString text = QString("The bit depth shoud be an integer between %1 and %2.");
        text = text.arg(constants::BIT_DEPTH_MIN_VALUE);
        text = text.arg(constants::BIT_DEPTH_MAX_VALUE);
        QMessageBox msgBox;
        msgBox.setText(text);
        msgBox.exec();
        return;
    }
    unsigned int bitDepth = bitDepthLE->text().toULong();
    minLE->setText("0");
    maxLE->setText(QString::number(math::ipow(2, bitDepth) - 1));
}

void IntensityDialog::setFrameMinMax()
{
    uint16_t min, max;
    movie->getFrameIntensityMinMax(frameIndex, min, max);
    minLE->setText(QString::number(min));
    maxLE->setText(QString::number(max));
}

void IntensityDialog::setMovieMinMax()
{
    progressWindow = new ProgressWindow(this);
    progressWindow->setWindowTitle("Calculating min and max...");
    progressWindow->setNStepsPtr(&(movie->nFrames));
    movie->currIndex = 0;
    progressWindow->setStepPtr(&(movie->currIndex));
    progressWindow->open();

    movieIntensityMinMaxWorker = new MovieIntensityMinMaxWorker(movie,
                                                                movieMin,
                                                                movieMax);
    taskThread = new QThread;
    movieIntensityMinMaxWorker->moveToThread(taskThread);
    connect(taskThread, &QThread::started,
            movieIntensityMinMaxWorker, &MovieIntensityMinMaxWorker::getIntensityMinMax);
    connect(movieIntensityMinMaxWorker, &MovieIntensityMinMaxWorker::finished,
            this, &IntensityDialog::onGetIntensityMinMaxFinish);
    taskThread->start();
}

void IntensityDialog::onGetIntensityMinMaxFinish()
{
    progressWindow->hide();
    delete progressWindow;
    disconnect(taskThread, &QThread::started,
               movieIntensityMinMaxWorker,
               &MovieIntensityMinMaxWorker::getIntensityMinMax);
    disconnect(movieIntensityMinMaxWorker,
               &MovieIntensityMinMaxWorker::finished,
               this, &IntensityDialog::onGetIntensityMinMaxFinish);
    // Not sure that the following is entirely safe.  For example, what if a new
    // thread is created before the old objects are actually deleted?
    taskThread->quit();
    movieIntensityMinMaxWorker->deleteLater();
    taskThread->deleteLater();
    taskThread->wait();

    minLE->setText(QString::number(movieMin));
    maxLE->setText(QString::number(movieMax));
}
