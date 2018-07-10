/*
 * This file is part of the particle tracking software CorrTrack.
 *
 * Copyright 2018 Nicolas Bruot and CNRS
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


#include <QApplication>
#include "constants.h"
#include "corrtrackwindow.h"


int main(int argc, char **argv)
{
    // The program is not designed for potentially very strange platforms:
    assert(CHAR_BIT == 8);
    assert(CHAR_BIT * sizeof(float) == 32);

    QApplication app (argc, argv);

    CorrTrackWindow window;
    window.show();

    return app.exec();
}
