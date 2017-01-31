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


#include "intensitydialog.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QWidget>
#include <QIntValidator>
#include <QMessageBox>
#include "constants.h"


IntensityDialog::IntensityDialog(const bool bitDepthSet,
                                 const unsigned int bitDepth,
                                 const unsigned long intensityMin,
                                 const unsigned long intensityMax,
                                 QWidget* parent)
    : OKCancelDialog(parent),
      bitDepthRB{new QRadioButton("Set bit depth", this)},
      minMaxIntensitiesRB{new QRadioButton("Set intensity boundaries", this)},
      bitDepthWidget{new QWidget(this)},
      minMaxIntensitiesWidget{new QWidget(this)},
      bitDepthLE{new QLineEdit(QString::number(bitDepth), this)},
      intensityMinLE{new QLineEdit(QString::number(intensityMin), this)},
      intensityMaxLE{new QLineEdit(QString::number(intensityMax), this)}

{
    setWindowTitle("Image intensity");

    const int lineEditWidth = 50;

    QIntValidator *bitDepthValidator = new QIntValidator(constants::BIT_DEPTH_MIN_VALUE,
                                                         constants::BIT_DEPTH_MAX_VALUE,
                                                         this);
    QIntValidator *intensityValidator = new QIntValidator(0,
                                                          constants::INTENSITY_MAX_VALUE,
                                                          this);
    bitDepthLE->setValidator(bitDepthValidator);
    intensityMinLE->setValidator(intensityValidator);
    intensityMaxLE->setValidator(intensityValidator);

    QHBoxLayout *bitDepthLayout = new QHBoxLayout(bitDepthWidget);
    bitDepthLE->setFixedWidth(lineEditWidth);
    bitDepthLayout->addItem(new QSpacerItem(constants::NESTED_LISTS_TAB_SPACING, 1));
    QString labelText = QString("Bit depth (")
            + QString::number(constants::BIT_DEPTH_MIN_VALUE)
            + QString("-") + QString::number(constants::BIT_DEPTH_MAX_VALUE)
            + QString(")");
    bitDepthLayout->addWidget(new QLabel(labelText, this));
    bitDepthLayout->addWidget(bitDepthLE);
    bitDepthLayout->addStretch();

    QHBoxLayout *minMaxIntensitiesLayout = new QHBoxLayout(minMaxIntensitiesWidget);
    intensityMinLE->setFixedWidth(lineEditWidth);
    intensityMaxLE->setFixedWidth(lineEditWidth);
    minMaxIntensitiesLayout->addItem(new QSpacerItem(constants::NESTED_LISTS_TAB_SPACING, 1));
    labelText = QString("Min (0-")
            + QString::number(constants::INTENSITY_MAX_VALUE)
            + QString(")");
    minMaxIntensitiesLayout->addWidget(new QLabel(labelText, this));
    minMaxIntensitiesLayout->addWidget(intensityMinLE);
    labelText = QString("Max (0-")
            + QString::number(constants::INTENSITY_MAX_VALUE)
            + QString(")");
    minMaxIntensitiesLayout->addWidget(new QLabel(labelText, this));
    minMaxIntensitiesLayout->addWidget(intensityMaxLE);
    minMaxIntensitiesLayout->addStretch();

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(bitDepthRB);
    mainLayout->addWidget(bitDepthWidget);
    mainLayout->addWidget(minMaxIntensitiesRB);
    mainLayout->addWidget(minMaxIntensitiesWidget);
    setLayout(mainLayout);

    bitDepthRB->setChecked(bitDepthSet);
    minMaxIntensitiesRB->setChecked(!bitDepthSet);

    updateIntensityWidgets();

    // These two lines connect the button push to IntensityDialog::ok() instead
    // of OKCancelDialog::ok().
    disconnect(okButton, SIGNAL(clicked()), this, SLOT(ok()));
    connect(okButton, SIGNAL(clicked()), this, SLOT(ok()));

    connect(bitDepthRB, SIGNAL(clicked()), this, SLOT(updateIntensityWidgets()));
    connect(minMaxIntensitiesRB, SIGNAL(clicked()), this, SLOT(updateIntensityWidgets()));
}

bool IntensityDialog::getBitDepthSet() const
{
    return bitDepthRB->isChecked();
}

unsigned int IntensityDialog::getBitDepth() const
{
    return bitDepthLE->text().toUInt();
}

unsigned long IntensityDialog::getIntensityMin() const
{
    return intensityMinLE->text().toULong();
}

unsigned long IntensityDialog::getIntensityMax() const
{
    return intensityMaxLE->text().toULong();
}

void IntensityDialog::updateIntensityWidgets()
{
    if (bitDepthRB->isChecked())
    {
        bitDepthWidget->setEnabled(true);
        minMaxIntensitiesWidget->setEnabled(false);
    }
    else
    {
        bitDepthWidget->setEnabled(false);
        minMaxIntensitiesWidget->setEnabled(true);
    }
}

void IntensityDialog::ok()
{
    if (!getBitDepthSet() && getIntensityMin() >= getIntensityMax())
    {
        QMessageBox msgBox;
        msgBox.setText("The maximum intensity should be strictly lower than the minimum intensity.");
        msgBox.exec();
        return;
    }
    return OKCancelDialog::ok();
}
