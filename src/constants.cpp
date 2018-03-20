/*
 * This file is part of the particle tracking software CorrTrack.
 *
 * Copyright 2016-2018 Nicolas Bruot
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


#include "constants.h"


#include <QPen>
#include <QBrush>


namespace constants
{
    const char* const VERSION = "2.9";
    const char* const TARGET_VERSION = "2.10";

    const char* const ORGANIZATION = "CorrTrack";
    const char* const APP_NAME = "CorrTrack";

    const double ZOOM_BASE = 4.0 / 3.0;
    const int ZOOM_POW_MIN = -12;
    const int ZOOM_POW_MAX = 12;

    const qreal POINT_RADIUS = 2.0;

    const QPen POINT_QPEN = QPen(Qt::NoPen);
    const QBrush POINT_QBRUSH = QBrush(Qt::red);
    const QPen INNER_RECT_QPEN = QPen(Qt::blue);
    const QPen OUTER_RECT_QPEN = QPen(Qt::red);

    const qreal POINT_Z_VALUE = 2.0;
    const qreal INNER_RECT_Z_VALUE = 4.0;
    const qreal OUTER_RECT_Z_VALUE = 1.0;
    const qreal FRAME_Z_VALUE = 0.0;
    const qreal CORRELATION_Z_VALUE = 3.0;

    const double MIN_FRAMERATE = 0.1;
    const double MAX_FRAMERATE = 1e6;
    const int FRAMERATE_DECIMALS = 8;

    const double DISPLAY_REFRESH_RATE = 20.0;

    const int PROGRESSBAR_REFRESH_PERIOD = 60.0;

    const int NESTED_LISTS_TAB_SPACING = 25;

    const int BIT_DEPTH_MIN_VALUE = 1;
    const int BIT_DEPTH_MAX_VALUE = 16;
    const int INTENSITY_MAX_VALUE = 65535;
}
