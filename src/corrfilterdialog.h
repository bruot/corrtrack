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
#include <QString>
#include "okcanceldialog.h"


class CorrFilterDialog : public OKCancelDialog
{
    Q_OBJECT

private:
    QLineEdit *filterWindowWidthLE;
    QLineEdit *filterWindowHeightLE;
    QLineEdit *filterFileLE;
    QLineEdit *fitRadiusLE;

private slots:
    void chooseFilterFile();

public:
    explicit CorrFilterDialog(const unsigned int filterWindowWidth,
                              const unsigned int filterWindowHeight,
                              const QString filterFile,
                              const double fitRadius,
                              const QString newLastFilterFolder,
                              const QString newLastFolder,
                              QWidget* parent = 0);
    unsigned int getFilterWindowWidth() const;
    unsigned int getFilterWindowHeight() const;
    double getFitRadius() const;
    QString getFilterFile() const;
    QString lastFilterFolder;
    QString lastFolder;
};
