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


#include <QPen>
#include <QBrush>


namespace constants
{
    extern const char* const VERSION;
    extern const char* const TARGET_VERSION;

    extern const char* const APP_NAME;

    extern const double ZOOM_BASE;
    extern const int ZOOM_POW_MIN;
    extern const int ZOOM_POW_MAX;

    extern const qreal POINT_RADIUS;

    extern const QPen POINT_QPEN;
    extern const QBrush POINT_QBRUSH;
    extern const QPen INNER_RECT_QPEN;
    extern const QPen OUTER_RECT_QPEN;

    extern const qreal POINT_Z_VALUE;
    extern const qreal INNER_RECT_Z_VALUE;
    extern const qreal OUTER_RECT_Z_VALUE;
    extern const qreal FRAME_Z_VALUE;
    extern const qreal CORRELATION_Z_VALUE;

    extern const double MIN_FRAMERATE;
    extern const double MAX_FRAMERATE;
    extern const int FRAMERATE_DECIMALS;

    extern const double DISPLAY_REFRESH_RATE;

    extern const int PROGRESSBAR_REFRESH_PERIOD;

    extern const int NESTED_LISTS_TAB_SPACING;

    extern const int BIT_DEPTH_MIN_VALUE;
    extern const int BIT_DEPTH_MAX_VALUE;
    extern const int INTENSITY_MAX_VALUE;
}
