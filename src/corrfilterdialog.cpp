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


#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDir>
#include <QIntValidator>
#include <QMessageBox>
#include <QString>
#include <QDoubleValidator>
#include "corrfilterdialog.h"
#include "constants.h"


CorrFilterDialog::CorrFilterDialog(const unsigned int filterWindowWidth,
                                   const unsigned int filterWindowHeight,
                                   const QString filterFile,
                                   const double fitRadius,
                                   const QString newLastFilterFolder,
                                   const QString newLastFolder,
                                   QWidget *parent)
    : OKCancelDialog(parent),
      filterWindowWidthLE{new QLineEdit(this)},
      filterWindowHeightLE{new QLineEdit(this)},
      filterFileLE{new QLineEdit(this)},
      fitRadiusLE{new QLineEdit(this)},
      lastFilterFolder{newLastFilterFolder},
      lastFolder{newLastFolder}
{
    setWindowTitle("Correlation Filter");

    QIntValidator *widthValidator = new QIntValidator(1,
                                                      constants::FILTER_WIDTH_MAX_VALUE,
                                                      this);
    filterWindowWidthLE->setValidator(widthValidator);
    QIntValidator *heightValidator = new QIntValidator(1,
                                                       constants::FILTER_HEIGHT_MAX_VALUE,
                                                       this);
    filterWindowHeightLE->setValidator(heightValidator);
    QDoubleValidator *fitRadiusValidator = new QDoubleValidator(0.0,
                                                                constants::FILTER_FIT_RADIUS_MAX_VALUE,
                                                                constants::FILTER_FIT_RADIUS_MAX_DECIMALS,
                                                                this);
    fitRadiusLE->setValidator(fitRadiusValidator);

    QLabel *filterWindowLabel = new QLabel("Correlation window");
    QLabel *filterWindowWidthLabel = new QLabel("Width (px)");
    QLabel *filterWindowHeightLabel = new QLabel("Height (px)");
    filterWindowWidthLE->setText(QString::number(filterWindowWidth));
    filterWindowHeightLE->setText(QString::number(filterWindowHeight));
    QVBoxLayout *filterLabelsLayout = new QVBoxLayout;
    filterLabelsLayout->addWidget(filterWindowWidthLabel);
    filterLabelsLayout->addWidget(filterWindowHeightLabel);
    QVBoxLayout *filterEditsLayout = new QVBoxLayout;
    filterEditsLayout->addWidget(filterWindowWidthLE);
    filterEditsLayout->addWidget(filterWindowHeightLE);
    QHBoxLayout *filterWindowLayout = new QHBoxLayout;
    filterWindowLayout->addLayout(filterLabelsLayout);
    filterWindowLayout->addLayout(filterEditsLayout);

    QHBoxLayout *filterFileLayout = new QHBoxLayout;
    QLabel *filterFileLabel = new QLabel("Filter file");
    filterFileLE->setText(filterFile);
    QPushButton *filterFileButton = new QPushButton("...");
    filterFileButton->setFixedWidth(40);
    filterFileLayout->addWidget(filterFileLabel);
    filterFileLayout->addWidget(filterFileLE);
    filterFileLayout->addWidget(filterFileButton);

    QVBoxLayout *filterOthersLabelsLayout = new QVBoxLayout;
    QLabel *fitRadiusLabel = new QLabel("Fit radius (px)");
    filterOthersLabelsLayout->addWidget(fitRadiusLabel);
    fitRadiusLE->setText(QString::number(fitRadius));
    QVBoxLayout *filterOthersEditsLayout = new QVBoxLayout;
    filterOthersEditsLayout->addWidget(fitRadiusLE);
    QHBoxLayout *filterOthersLayout = new QHBoxLayout;
    filterOthersLayout->addLayout(filterOthersLabelsLayout);
    filterOthersLayout->addLayout(filterOthersEditsLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(filterWindowLabel);
    mainLayout->addLayout(filterWindowLayout);
    mainLayout->addLayout(filterFileLayout);
    mainLayout->addLayout(filterOthersLayout);
    setLayout(mainLayout);

    connect(filterFileButton, SIGNAL(clicked()),
            this, SLOT(chooseFilterFile()));
}

void CorrFilterDialog::chooseFilterFile()
{
    QString fileName;

    QString folder = lastFilterFolder.isEmpty() ? lastFolder : lastFilterFolder;

    fileName = QFileDialog::getOpenFileName(this,
        tr("Open File"), folder,
           tr("Text Data Files (*.dat)"));

    if (!fileName.isEmpty())
    {
        filterFileLE->setText(fileName);
        lastFilterFolder = QString(QFileInfo(fileName).path());
        lastFolder = lastFilterFolder;
    }
}

unsigned int CorrFilterDialog::getFilterWindowWidth() const
{
    return filterWindowWidthLE->text().toUInt();
}

unsigned int CorrFilterDialog::getFilterWindowHeight() const
{
    return filterWindowHeightLE->text().toUInt();
}

double CorrFilterDialog::getFitRadius() const
{
    return fitRadiusLE->text().toDouble();
}

QString CorrFilterDialog::getFilterFile() const
{
    return filterFileLE->text();
}

void CorrFilterDialog::ok()
{
    // Validate fields
    QMessageBox *msgBox = new QMessageBox(this);
    int pos;

    pos = filterWindowWidthLE->cursorPosition();
    if (filterWindowWidthLE->validator()->validate(filterWindowWidthLE->text(), pos) != QValidator::Acceptable)
    {
        msgBox->setText(QString("Filter width value outside acceptable range (1-%1).").arg(constants::FILTER_WIDTH_MAX_VALUE));
        msgBox->exec();
        return;
    }

    pos = filterWindowHeightLE->cursorPosition();
    if (filterWindowHeightLE->validator()->validate(filterWindowHeightLE->text(), pos) != QValidator::Acceptable)
    {
        msgBox->setText(QString("Filter height value outside acceptable range (1-%1).").arg(constants::FILTER_HEIGHT_MAX_VALUE));
        msgBox->exec();
        return;
    }

    pos = fitRadiusLE->cursorPosition();
    if (fitRadiusLE->validator()->validate(fitRadiusLE->text(), pos) != QValidator::Acceptable)
    {
        msgBox->setText(QString("Fit radius value outside acceptable range (0-%1).").arg(constants::FILTER_FIT_RADIUS_MAX_VALUE));
        msgBox->exec();
        return;
    }

    return OKCancelDialog::ok();
}
