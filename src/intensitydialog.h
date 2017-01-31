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


#pragma once


#include <QDialog>
#include <QLineEdit>
#include <QWidget>
#include <QRadioButton>
#include "okcanceldialog.h"


class IntensityDialog : public OKCancelDialog
{
    Q_OBJECT

private:
    QRadioButton *bitDepthRB;
    QRadioButton *minMaxIntensitiesRB;
    QWidget *bitDepthWidget;
    QWidget *minMaxIntensitiesWidget;
    QLineEdit *bitDepthLE;
    QLineEdit *intensityMinLE;
    QLineEdit *intensityMaxLE;


private slots:
    void ok() override;
    void updateIntensityWidgets();

public:
    explicit IntensityDialog(const bool bitDepthSet,
                             const unsigned int bitDepth,
                             const unsigned long intensityMin,
                             const unsigned long intensityMax,
                             QWidget* parent = 0);
    bool getBitDepthSet() const;
    unsigned int getBitDepth() const;
    unsigned long getIntensityMin() const;
    unsigned long getIntensityMax() const;
};
