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
#include <QToolButton>
#include <QThread>
#include "okcanceldialog.h"
#include "corrtrackwindow.h"
#include "movie/movie.h"
#include "progresswindow.h"
#include "movieintensityminmaxworker.h"


class IntensityDialog : public OKCancelDialog
{
    Q_OBJECT

private:
    size_t frameIndex;
    Movie* movie;

    QRadioButton *minMaxRB;
    QRadioButton *autoVarRB;
    //
    QWidget *minMaxWidget;
    QLineEdit *minLE;
    QLineEdit *maxLE;
    QLineEdit *bitDepthLE;
    QToolButton *bitDepthBtn;
    QToolButton *autoFrameBtn;
    QToolButton *autoMovieBtn;

    QThread* taskThread;
    MovieIntensityMinMaxWorker* movieIntensityMinMaxWorker;
    ProgressWindow* progressWindow;

    uint16_t movieMin;
    uint16_t movieMax;

    bool isMinMaxBitDepth() const;

private slots:
    void ok() override;
    void updateIntensityWidgets();
    void setBitDepthMinMax();
    void setFrameMinMax();
    void setMovieMinMax();
    void onGetIntensityMinMaxFinish();

public:
    explicit IntensityDialog(CorrTrackWindow::IntensityMode intensityMode,
                             const uint16_t intensityMin,
                             const uint16_t intensityMax,
                             Movie* movie,
                             size_t frameIndex,
                             QWidget* parent = 0);
    CorrTrackWindow::IntensityMode getIntensityMode() const;
    unsigned int getBitDepth() const;
    unsigned long getIntensityMin() const;
    unsigned long getIntensityMax() const;
};
