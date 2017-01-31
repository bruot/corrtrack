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


#include <boost/cstdint.hpp>
#include <movie/base/frame.h>


// Stores an image as an array of double.
class ImageD
{
public:
    explicit ImageD(const Frame<uint8_t> * const frame);
    explicit ImageD(const uint8_t * const data,
                    const unsigned int width, const unsigned int height);
    explicit ImageD(const unsigned int width, const unsigned int height);
    ~ImageD();
    ImageD(const ImageD&) =delete;
    ImageD& operator=(const ImageD&) =delete;
    ImageD(ImageD&&) =delete;
    ImageD& operator=(ImageD&&) =delete;

    double getPixelIntensity(const unsigned int x, const unsigned int y) const;
    void setPixelIntensity(const unsigned int x,
                           const unsigned int y,
                           const double value);
    uint8_t* rescaledPixelsData() const;

    unsigned int width;
    unsigned int height;
    double *pixelsData;
};
