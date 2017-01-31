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


#include <QObject>
#include <QDialog>
#include <QProgressBar>
#include <QCloseEvent>
#include <QKeyEvent>


class ProgressWindow : public QDialog
{
    Q_OBJECT

private:
    QProgressBar* progressBar;
    QTimer* timer;
    size_t* step;
    size_t* nSteps;

private slots:
    void refresh();

public:
    explicit ProgressWindow(QWidget* parent = 0);
    ~ProgressWindow();
    void setNStepsPtr(size_t * nSteps);
    void setStepPtr(size_t * step);
    void open() override;
    void hide();

protected:
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void closeEvent(QCloseEvent* event) override;
};
