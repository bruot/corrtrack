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


#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include "math.h"
#include "constants.h"
#include "zoomdialog.h"


ZoomDialog::ZoomDialog(const int zoomIndex, QWidget* parent)
    : OKCancelDialog(parent),
      zoomFactorLabel{new QLabel(this)},
      slider{new QSlider(this)}
{
    setWindowTitle("Zoom");

    QLabel *zoomLabel = new QLabel("Zoom factor:");

    slider->setOrientation(Qt::Horizontal);
    slider->setRange(constants::ZOOM_POW_MIN, constants::ZOOM_POW_MAX);
    slider->setValue(zoomIndex);

    updateZoomFactorLabel(zoomIndex);

    QHBoxLayout *sliderLayout = new QHBoxLayout;
    sliderLayout->addWidget(slider);
    sliderLayout->addWidget(zoomFactorLabel);

    QVBoxLayout *contentsLayout = new QVBoxLayout;
    contentsLayout->addWidget(zoomLabel);
    contentsLayout->addLayout(sliderLayout);
    setLayout(contentsLayout);

    connect(slider, SIGNAL(valueChanged(int)),
            this, SLOT(updateZoomFactorLabel(const int)));
}

void ZoomDialog::updateZoomFactorLabel(const int zoomIndex)
{
    zoomFactorLabel->setText(QString::number(pow(constants::ZOOM_BASE, zoomIndex), 'f', 3));
}

int ZoomDialog::getZoomIndex() const
{
    return slider->value();
}
