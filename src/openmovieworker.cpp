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


#include <exception>
#include <QString>
#include "math/corrtrackanalyser.h"
#include "io/exceptions/ioexception.h"
#include "movie/movie.h"
#include "openmovieworker.h"


OpenMovieWorker::OpenMovieWorker(CorrTrackAnalyser* analyser,
                                 const QString fileName,
                                 QObject *parent)
    : QObject(parent), analyser{analyser}, fileName{fileName}
{}

void OpenMovieWorker::openMovie()
{
    QString errorMsg;
    try
    {
        analyser->setMovie(fileName.toStdString());
    }
    catch (IOException &e)
    {
        errorMsg = QString(e.what());
    }
    catch (Movie::MovieException &e)
    {
        errorMsg = QString(e.what());
    }

    QString msg;
    if (!errorMsg.isEmpty())
    {
        msg = QString("Error while opening movie:\n");
        msg += errorMsg;
        msg += QString("\nAborting.");
    }

    emit finishedWithMessage(fileName, msg);
    emit finished();
}
