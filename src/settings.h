/*
 * This file is part of the particle tracking software CorrTrack.
 *
 * Copyright 2018 Nicolas Bruot
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


#include <QString>
#include <QSettings>


class Settings : public QObject
{
    Q_OBJECT

public:
    Settings(QObject *parent = 0);
    ~Settings();

    bool highlightMinIntensity;
    bool highlightMaxIntensity;

public slots:
    void saveSettings();

private:
    QSettings *qsettings;

    template<typename T>
        T getValue(QString field, T defaultValue);
};
