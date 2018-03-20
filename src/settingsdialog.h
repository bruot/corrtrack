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


#include <QObject>
#include <QCheckBox>
#include "okcanceldialog.h"
#include "settings.h"


class SettingsDialog : public OKCancelDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(const bool highlightMinIntensity,
                            const bool highlightMaxIntensity,
                            QWidget* parent = 0);
    const bool getHighlightMinIntensity();
    const bool getHighlightMaxIntensity();


private:
    Settings* settings;

    QCheckBox* highlightMinIntensityCB;
    QCheckBox* highlightMaxIntensityCB;
};
