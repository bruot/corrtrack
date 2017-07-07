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
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "base/frame.h"
#include "base/movieformats.h"


class Movie
{
private:
    void deleteAllFrames();
    void loadRawmMovie();
    void loadXiseqMovie();
    void loadImageMovie();
    void loadTiffMovie();
    boost::property_tree::ptree getPropertyTree(const std::string fileName) const;
    boost::property_tree::ptree getChild(const boost::property_tree::ptree pt,
                                         const char * const key) const;
    template<typename outputType>
        void setXmlVar(const boost::property_tree::ptree pt,
                       const char* const name,
                       outputType &variable,
                       const bool isOptional = false) const;
    unsigned int intLog10(const unsigned int value) const;
    MovieFormats::PixelFmt safeStrToPixelFmt(const std::string pixelFmtStr) const;
    MovieFormats::PixelFmt safeInt32ToPixelFmt(const uint32_t pixelFmtInt) const;

    struct frameMetadata
    {
        // This is used by all movie formats, but the fileName field may be
        // unused.
        unsigned long long timestamp;
        std::string fileName;
    };

    class RegExpNoMatchException : public std::exception
    {
    public:
        virtual const char* what() const noexcept override;
    };

public:
    enum class Format
    {
        Image,
        Xiseq,
        Rawm,
        Tiff,
    };

    Movie();
    ~Movie();
    Movie(const Movie&) =delete;
    Movie& operator=(const Movie&) =delete;
    Movie(Movie&&) =delete;
    Movie& operator=(Movie&&) =delete;

    void openMovie(const std::string fileName);
    void extractTiff(const size_t frameIndex) const;
    void extractTiffs();

    void getIntensityMinMax(uint16_t& min, uint16_t& max) const;
    void getFrameIntensityMinMax(size_t frameIndex,
                                 uint16_t& min, uint16_t& max) const;
    uint8_t* frameData8(const size_t i,
                        const unsigned int customBitDepth) const;
    uint8_t* frameData8(const size_t i,
                        const unsigned int minValue,
                        const unsigned int maxValue) const;

    class MovieException : public std::exception
    {
    public:
        MovieException(const char * const msg);
        virtual const char* what() const noexcept override;
    private:
        char* err_msg;
    };

    std::string fileName;
    Format format;
    unsigned int bitsPerSample;
    unsigned int bitDepth;
    unsigned int width;
    unsigned int height;
    size_t nFrames;
    double framerate;
    std::vector<Frame<uint8_t>> frames8;
    std::vector<Frame<uint16_t>> frames16;
    std::vector<unsigned long> timestamps;
    mutable size_t currIndex;
};
