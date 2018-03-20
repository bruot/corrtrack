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


#include <QVariant>
#include <QMessageBox>
#include <QSettings>
#include "constants.h"
#include "settings.h"


Settings::Settings(QObject *parent) : QObject(parent)
{
    qsettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                              constants::ORGANIZATION, constants::APP_NAME);
    highlightMinIntensity = getValue("Display/HighlightMinIntensity", true);
    highlightMaxIntensity = getValue("Display/HighlightMaxIntensity", true);
}


Settings::~Settings()
{
    delete qsettings;
}

template<typename T>
    T Settings::getValue(QString field, T defaultValue)
{
    QVariant val = qsettings->value(field, defaultValue);
    if (val.canConvert<T>())
    {
        if (val.convert(qMetaTypeId<T>()))
            return val.value<T>();
    }
    return defaultValue;
}

void Settings::saveSettings()
{
    qsettings->setValue("Display/HighlightMinIntensity", highlightMinIntensity);
    qsettings->setValue("Display/HighlightMaxIntensity", highlightMaxIntensity);
}
