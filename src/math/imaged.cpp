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


#include <algorithm>
#include "imaged.h"


ImageD::ImageD(const Frame<uint8_t> * const frame)
    : width{frame->width}, height{frame->height},
      pixelsData{new double[width * height]}
{
    for (unsigned int k = 0; k < width * height; k++)
    {
        pixelsData[k] = (double) frame->pixelsData[k];
    }
}

ImageD::ImageD(const uint8_t * const data,
               const unsigned int width, const unsigned int height)
    : width{width}, height{height},
      pixelsData{new double[width * height]}
{
    for (unsigned int k = 0; k < width * height; k++)
    {
        pixelsData[k] = (double) data[k];
    }
}

ImageD::ImageD(const unsigned int width, const unsigned int height)
    : width{width}, height{height},
      pixelsData{new double[width * height]}
{
    // Initializes an ImageD instance with an array of zeros.
}

ImageD::~ImageD()
{
    delete[] pixelsData;
}

double ImageD::getPixelIntensity(const unsigned int x,
                                 const unsigned int y) const
{
    // First pixel is (0,0).

    unsigned int i = width * y + x;
    return pixelsData[i];
}

void ImageD::setPixelIntensity(const unsigned int x, const unsigned int y,
                               const double value)
{
    // First pixel is (0,0).

    unsigned int i = width * y + x;
    pixelsData[i] = value;
}

uint8_t* ImageD::rescaledPixelsData() const
{
    // Returns a pixels map as u_int8_t and rescaled between 0 and 255.

    uint8_t *rescaledPixelsData = new uint8_t[width * height];
    const double minVal = *(std::min_element(pixelsData,
                                              pixelsData + width * height));
    const double maxVal = *(std::max_element(pixelsData,
                                              pixelsData + width * height));
    for (unsigned int k = 0; k < width * height; k++)
    {
        // Little hack here: The max gives 256, which is invalid, but is
        // conveniently represented as a black dot.
        rescaledPixelsData[k] = (uint8_t) (256.0 * (pixelsData[k] - minVal)
                                           / (maxVal - minVal));
    }

    return rescaledPixelsData;
}
