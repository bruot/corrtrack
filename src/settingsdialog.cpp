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


#include <QWidget>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "settingsdialog.h"
#include "settings.h"
#include "okcanceldialog.h"


SettingsDialog::SettingsDialog(const bool highlightMinIntensity,
                               const bool highlightMaxIntensity,
                               QWidget* parent)
    : OKCancelDialog(parent),
      highlightMinIntensityCB{new QCheckBox("Highlight under exposed pixels")},
      highlightMaxIntensityCB{new QCheckBox("Highlight over exposed pixels")}
{
    setWindowTitle("Settings");

    highlightMinIntensityCB->setChecked(highlightMinIntensity);
    highlightMaxIntensityCB->setChecked(highlightMaxIntensity);

    QVBoxLayout *intensitiesHighlights = new QVBoxLayout;
    QLabel *intensitiesHighlightsLabel = new QLabel("Under and over exposed pixels:");
    intensitiesHighlights->addWidget(intensitiesHighlightsLabel);
    intensitiesHighlights->addWidget(highlightMinIntensityCB);
    intensitiesHighlights->addWidget(highlightMaxIntensityCB);

    setLayout(intensitiesHighlights);
}

const bool SettingsDialog::getHighlightMinIntensity()
{
    return highlightMinIntensityCB->isChecked();
}

const bool SettingsDialog::getHighlightMaxIntensity()
{
    return highlightMaxIntensityCB->isChecked();
}
