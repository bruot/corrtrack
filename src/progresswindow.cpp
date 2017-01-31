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


#include <QWindow>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QKeyEvent>
#include <QTimer>
#include "constants.h"
#include "progresswindow.h"


ProgressWindow::ProgressWindow(QWidget* parent)
    : QDialog(parent, Qt::CustomizeWindowHint|Qt::WindowTitleHint),
      progressBar{new QProgressBar(this)},
      timer{new QTimer(this)},
      step{nullptr},
      nSteps{nullptr}
{
    // Qt::CustomizeWindowHint|Qt::WindowTitleHint above disables the close
    // button.

    progressBar->setMinimum(0);
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(progressBar);
    this->setLayout(mainLayout);

    timer->setInterval(constants::PROGRESSBAR_REFRESH_PERIOD);
    connect(timer, &QTimer::timeout, this, &ProgressWindow::refresh);
}

ProgressWindow::~ProgressWindow()
{
    timer->stop();
}

void ProgressWindow::setNStepsPtr(size_t * nSteps)
{
    this->nSteps = nSteps;
}

void ProgressWindow::setStepPtr(size_t * step)
{
    this->step = step;
}

void ProgressWindow::refresh()
{
    progressBar->setMaximum((int) *nSteps);
    progressBar->setValue((int) *step);
}

void ProgressWindow::open()
{
    refresh();
    timer->start();
    QDialog::open();
}

void ProgressWindow::hide()
{
    timer->stop();
    QDialog::hide();
}

void ProgressWindow::closeEvent(QCloseEvent* event)
{
    event->ignore();
}

void ProgressWindow::keyPressEvent(QKeyEvent*)
{
    // Disables key events on this window.
}
