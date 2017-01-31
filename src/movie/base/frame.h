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


#include <string>
#include <exception>
#include "movieformats.h"


template<typename PixelDataType>
class Frame
{
public:
    Frame();
    ~Frame();
    Frame(const Frame&) =delete;
    Frame& operator=(const Frame&) =delete;
    Frame(Frame&&) =delete;
    Frame& operator=(Frame&&) =delete;

    void load(const std::string filename,
              const MovieFormats::PixelFmt pixelFmt = MovieFormats::PixelFmt::Mono8,
              const unsigned long timestamp = 0);
    void load(const PixelDataType* const pixelsData,
              const uint32_t width, const uint32_t height,
              const unsigned long timestamp = 0);

    PixelDataType getPixelIntensity(const unsigned int x,
                                    const unsigned int y) const;
    void save(const std::string fileName) const;

    unsigned long timestamp;
    unsigned int width;
    unsigned int height;
    PixelDataType *pixelsData;

    class FrameLoadException : public std::exception
    {
    public:
        virtual const char* what() const noexcept override;
    };

    class FrameSaveException : public std::exception
    {
    public:
        virtual const char* what() const noexcept override;
    };
};
