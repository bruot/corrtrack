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
#include "extracttiffsworker.h"


ExtractTiffsWorker::ExtractTiffsWorker(CorrTrackAnalyser* analyser,
                                       QObject *parent)
    : QObject(parent), analyser{analyser}
{}

void ExtractTiffsWorker::extractTiffs()
{
    QString msg;
    try
    {
        analyser->movie->extractTiffs();
    }
    catch (std::ios_base::failure& e)
    {
        msg = QString("Error while extracting frames: ");
        msg += QString::fromStdString(e.what());
        msg += QString(" Aborting.");
    }
    if (msg.isEmpty())
        msg = QString("Done extracting TIFFs!");

    emit finishedWithMessage(msg);
    emit finished();
}
