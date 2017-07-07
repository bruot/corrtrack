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


#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <limits>
#include <exception>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "base/frame.h"
#include "base/frame.cpp" // needed because it is a template
#include "base/version.h"
#include "base/movieformats.h"
#include "movie.h"


namespace fs = boost::filesystem;
namespace pt = boost::property_tree;


Movie::MovieException::MovieException(const char * const msg)
    : std::exception()
{
    err_msg = (char *) malloc(1 + std::strlen(msg));
    strcpy(err_msg, msg);
}

const char* Movie::MovieException::what() const noexcept
{
    return err_msg;
}

const char* Movie::RegExpNoMatchException::what() const noexcept
{
    return "No match for regular expression.";
}

Movie::Movie()
    : fileName{""},
      bitsPerSample{NULL},
      bitDepth{NULL},
      width{NULL},
      height{NULL},
      nFrames{0},
      framerate{NULL},
      frames8{std::vector<Frame<uint8_t>>()},
      frames16{std::vector<Frame<uint16_t>>()},
      timestamps{std::vector<unsigned long>()},
      currIndex{0}
{
    format = Format::Image;
}

Movie::~Movie()
{
    deleteAllFrames();
}

void Movie::deleteAllFrames()
{
    if (bitsPerSample == 8)
        std::vector<Frame<uint8_t>>().swap(frames8); // Better than clear() as it frees the memory.
    else
        std::vector<Frame<uint16_t>>().swap(frames16);
    std::vector<unsigned long>().swap(timestamps);

    nFrames = 0;
}

void Movie::openMovie(const std::string fileName)
{
    // Resets some parameters
    deleteAllFrames();
    format = Format::Image;
    bitsPerSample = 0;
    bitDepth = 0;
    width = 0;
    height = 0;
    framerate = 0;
    currIndex = 0;

    this->fileName = fileName;

    // CHeck that the file exists
    if (!boost::filesystem::exists(fileName))
        throw MovieException("File does not exist.");

    fs::path path(fileName);
    fs::path ext = path.extension();

    if (ext == ".rawm")
    {
        format = Format::Rawm;
        loadRawmMovie();
    }
    else if (ext == ".xiseq")
    {
        format = Format::Xiseq;
        loadXiseqMovie();
    }
    else if (ext == ".tif" || ext == ".tiff")
    {
        format = Format::Tiff;
        loadTiffMovie();
    }
    else if ((ext == ".png") ||
             (ext == ".jpg") ||
             (ext == ".bmp")
            )
    {
        format = Format::Image;
        loadImageMovie();
    }
    else
        throw MovieException("Unknown file extension.");
    timestamps.reserve(nFrames);
    if (bitsPerSample == 8)
    {
        width = frames8.at(0).width;
        height = frames8.at(0).height;
        for (unsigned int i = 0; i < nFrames; i++)
            timestamps.push_back(frames8.at(i).timestamp);
    }
    else
    {
        width = frames16.at(0).width;
        height = frames16.at(0).height;
        for (unsigned int i = 0; i < nFrames; i++)
            timestamps.push_back(frames16.at(i).timestamp);
    }
}

pt::ptree Movie::getPropertyTree(std::string fileName) const
{
    pt::ptree pt;
    try
    {
        pt::read_xml(fileName, pt);
    }
    catch (pt::xml_parser_error)
    {
        throw MovieException("XML parser error.");
    }
    return pt;
}

pt::ptree Movie::getChild(const boost::property_tree::ptree pt,
                          const char* const key) const
{
    pt::ptree child;
    try
    {
        child = pt.get_child(key);
    }
    catch(pt::ptree_bad_path)
    {
        QString msg = QString("Key '%1' not found.").arg(key);
        throw MovieException(msg.toStdString().c_str());
    }
    return child;
}

template<typename outputType>
    void Movie::setXmlVar(const boost::property_tree::ptree pt,
                          const char * const name,
                          outputType &variable,
                          const bool isOptional) const
{
    try
    {
        variable = pt.get<outputType>(name);
    }
    catch (pt::ptree_bad_path)
    {
        if (!isOptional)
        {
            QString msg = QString("Key '%1' not found.").arg(name);
            throw MovieException(msg.toStdString().c_str());
        }
    }
    catch (pt::ptree_bad_data)
    {
        QString msg = QString("Error while reading key '%1'.").arg(name);
        throw MovieException(msg.toStdString().c_str());
    }
}

void Movie::loadRawmMovie()
{
    // Read header

    // getPropertyTree, getChild and setXmlVar are safe methods that throw a
    // MovieException if necessary.

    pt::ptree pt = getPropertyTree(fileName);

    // Get version
    std::string versionStr;
    setXmlVar<std::string>(pt, "movie_metadata.<xmlattr>.version", versionStr);
    Version version(versionStr);

    // Get pixel format and related parameters
    MovieFormats::PixelFmt pixelFmt;
    if (version < Version("1.3"))
    {
        std::string imgDataFormat;
        setXmlVar<std::string>(pt, "movie_metadata.header.image_data_format",
                               imgDataFormat);
        if (imgDataFormat != std::string("MONO8"))
            throw MovieException("Rawm metadata format of version lower than 1.3 only supports Mono8 images.");
        pixelFmt =  MovieFormats::PixelFmt::Mono8;
    }
    else
    {
        std::string pixelFmtStr;
        setXmlVar<std::string>(pt, "movie_metadata.header.pixel_format",
                               pixelFmtStr);
        pixelFmt = safeStrToPixelFmt(pixelFmtStr);
    }
    bitsPerSample = MovieFormats::PixelFmtBitsPerSample.at(pixelFmt);
    bitDepth = MovieFormats::PixelFmtBitDepth.at(pixelFmt);

    // Get endianness
    MovieFormats::Endianness endianness;

    if (version < Version("1.3"))
    {
        endianness = MovieFormats::Endianness::little;
    }
    else
    {
        std::string endiannessStr;
        setXmlVar<std::string>(pt, "movie_metadata.header.endianness",
                               endiannessStr);
        if (endiannessStr == std::string("little"))
            endianness = MovieFormats::Endianness::little;
        else if (endiannessStr == std::string("big"))
            endianness = MovieFormats::Endianness::big;
        else
            throw MovieException("Unknwown endianness.");
    }

    if (endianness == MovieFormats::Endianness::big)
        throw MovieException("Data stored in big endian is currently not supported.");

    uint32_t width;
    setXmlVar<uint32_t>(pt, "movie_metadata.header.width", width);
    uint32_t height;
    setXmlVar<uint32_t>(pt, "movie_metadata.header.height", height);

    try
    {
        setXmlVar<double>(pt, "movie_metadata.header.framerate", framerate);
    }
    catch (MovieException) {} // ignore if framerate is missing or unreadable

    std::vector<frameMetadata> framesMeta;
    pt::ptree ptFrames = getChild(pt, "movie_metadata.frames");
    for (auto it: ptFrames)
    {
        uint64_t timestamp;
        setXmlVar<uint64_t>(it.second, "<xmlattr>.timestamp", timestamp);
        frameMetadata fm;
        fm.timestamp = timestamp;
        framesMeta.push_back(fm);
    }

    if (framesMeta.size() == 0)
        throw MovieException("No frames found in XML file.");

    nFrames = (unsigned int) (framesMeta.size());
    if (bitsPerSample == 8)
        frames8 = std::vector<Frame<uint8_t>>(nFrames);
    else // bitsPerSample == 16
        frames16 = std::vector<Frame<uint16_t>>(nFrames);

    std::string rawFileName = fileName;
    // Since we are sure there is a dot in the filename, this line is fine:
    rawFileName.erase(rawFileName.find_last_of("."), std::string::npos);
    rawFileName.append(".raw");

    // Check file size
    uintmax_t rawFileSize;
    try
    {
        rawFileSize = fs::file_size(rawFileName);
    }
    catch (fs::filesystem_error)
    {
        throw MovieException("Could not read .raw file.");
    }
    if (rawFileSize != (uintmax_t) width * height * (bitsPerSample / 8) * nFrames)
        throw MovieException(".raw file size is inconsistent with movie format in .rawm file.");

    std::ifstream is(rawFileName, std::ifstream::binary);
    //
    if (is)
    {
        size_t bufferSize = width * height * (bitsPerSample / 8);
        unsigned char *buffer = new unsigned char[bufferSize];
        if (bitsPerSample == 8)
        {
            while (is.read((char*)buffer, bufferSize))
            {
                frames8.at(currIndex).load(buffer, width, height,
                                                   framesMeta.at(currIndex).timestamp);
                ++currIndex;
            }
        }
        else // bitsPerSample == 16
        {
            uint16_t* buffer16 = new uint16_t[width * height];
            while (is.read((char*)buffer, bufferSize))
            {
                for (size_t k = 0; k < width * height; k++)
                    buffer16[k] = ((uint16_t) (buffer[2*k+1]) << 8) + (uint16_t) (buffer[2*k]); // Little endian
                frames16.at(currIndex).load(buffer16, width, height,
                                                framesMeta.at(currIndex).timestamp);
                ++currIndex;
            }
            delete buffer16;
        }
        delete buffer;
    }
    else
        throw MovieException("Could not open .raw file.");
    is.close();
}

void Movie::loadXiseqMovie()
{
    pt::ptree pt = getPropertyTree(fileName);

    std::vector<frameMetadata> framesMeta;
    pt::ptree ptFrames = getChild(pt, "ImageSequence");
    for (auto it: ptFrames)
    {
        if (std::strcmp(it.first.data(), "file") == 0)
        {
            frameMetadata fm;
            std::string fileName = it.second.data();
            if (fileName.empty())
                throw MovieException("<file> key has no data.");
            fm.fileName = fileName;
            setXmlVar<uint64_t>(it.second, "<xmlattr>.timestamp", fm.timestamp);
            framesMeta.push_back(fm);
        }
    }

    // Determine pixel format.  If xiApiImg:format is not found, assume Mono8.
    std::string apiContextList;
    setXmlVar<std::string>(pt, "ImageSequence.imageMetadata.apiContextList",
                           apiContextList);
    std::istringstream iss(apiContextList);
    uint32_t formatInt = MovieFormats::PixelFmtToInt32.at(MovieFormats::PixelFmt::Mono8);
    for (std::string line; std::getline(iss, line);)
        if (boost::starts_with(line, "xiApiImg:format="))
        {
            boost::trim_right(line);
            try
            {
                formatInt = boost::lexical_cast<uint32_t>(line.substr(16, line.size()));
            }
            catch (const boost::bad_lexical_cast)
            {
                throw MovieException("Cannot read \"xiApiImg:format\" parameter.");
            }

            break;
        }

    MovieFormats::PixelFmt pixelFmt = safeInt32ToPixelFmt(formatInt);
    bitsPerSample = MovieFormats::PixelFmtBitsPerSample.at(pixelFmt);
    bitDepth = MovieFormats::PixelFmtBitDepth.at(pixelFmt);

    // Read frames
    fs::path p(fileName);
    fs::path framesDir = p.parent_path();
    fs::path frameFullPath;
    nFrames = (unsigned int) (framesMeta.size());
    if (bitsPerSample == 8)
        frames8 = std::vector<Frame<uint8_t>>(nFrames);
    else
        frames16 = std::vector<Frame<uint16_t>>(nFrames);
    for (currIndex = 0; currIndex < nFrames; ++currIndex)
    {
        frameFullPath = framesDir / framesMeta.at(currIndex).fileName;
        frameFullPath.make_preferred();

        std::string pathStr = frameFullPath.string();
        uint64_t timestamp = framesMeta.at(currIndex).timestamp;
        if (bitsPerSample == 8)
        {
            try
            {
                frames8.at(currIndex).load(pathStr, pixelFmt, timestamp);
            }
            catch (Frame<uint8_t>::FrameLoadException)
            {
                throw MovieException("Frame load exception.");
            }
        }
        else // bitsPerSample == 16
        {
            try
            {
                frames16.at(currIndex).load(pathStr, pixelFmt, timestamp);
            }
            catch (Frame<uint16_t>::FrameLoadException)
            {
                throw MovieException("Frame load exception.");
            }
        }
    }
}

void Movie::loadTiffMovie()
{
    // Loads a single TIFF image.

    nFrames = 1;

    // Get bits per sample from tif metadata
    TIFF *tif = TIFFOpen(fileName.c_str(), "r");
    if (tif)
    {
        uint16 tmpBitsPerSample;
        TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &tmpBitsPerSample);
        if (tmpBitsPerSample != 8 && tmpBitsPerSample != 16)
            throw MovieException("Only 8 and 16 bits per pixel sample are allowed.");
        bitsPerSample = (unsigned int) tmpBitsPerSample;
        // We don't know the bit depth, so here is a guess:
        bitDepth = bitsPerSample;
        TIFFClose(tif);
    }
    else
        throw MovieException("Could not open tif.");

    if (bitsPerSample == 8)
    {
        frames8 = std::vector<Frame<uint8_t>>(nFrames);
        try
        {
            frames8[0].load(fileName);
        }
        catch (Frame<uint8_t>::FrameLoadException)
        {
            throw MovieException("Could not load frame.");
        }
    }
    else
    {
        frames16 = std::vector<Frame<uint16_t>>(nFrames);
        try
        {
            frames16[0].load(fileName, MovieFormats::PixelFmt::Mono16);
        }
        catch (Frame<uint16_t>::FrameLoadException)
        {
            throw MovieException("Could not load frame.");
        }
    }
}

void Movie::loadImageMovie()
{
    // This only supports 8-bit images.

    nFrames = 1;
    frames8 = std::vector<Frame<uint8_t>>(nFrames);
    bitsPerSample = 8;
    bitDepth = 8;
    try
    {
        frames8[0].load(fileName);
    }
    catch (Frame<uint8_t>::FrameLoadException)
    {
        throw MovieException("Frame load exception.");
    }
}

void Movie::extractTiff(const size_t frameIndex) const
{
    fs::path path(fileName);
    fs::path basePath = fs::change_extension(path, "");

    std::string strFmt("_%0");
    strFmt += std::to_string(intLog10((unsigned int) nFrames));
    strFmt += "d.tif";

    boost::format frameNameFmt(strFmt);
    frameNameFmt % (frameIndex + 1);
    std::string tifPath = basePath.string() + frameNameFmt.str();

    if (bitsPerSample == 8)
        frames8.at(frameIndex).save(tifPath);
    else
        frames16.at(frameIndex).save(tifPath);
}

void Movie::extractTiffs()
{
    fs::path path(fileName);
    fs::path basePath = fs::change_extension(path, "");

    if(!boost::filesystem::create_directory(basePath))
    {
        throw std::ios_base::failure("Could not create directory for TIFFs");
    }

    std::string strFmt("%0");
    strFmt += std::to_string(intLog10((unsigned int) nFrames));
    strFmt += "d.tif";

    for (currIndex = 0;
         currIndex < nFrames;
         ++currIndex)
    {
        boost::format baseNameFmt(strFmt);
        baseNameFmt % (currIndex + 1);
        fs::path tifPath = basePath / baseNameFmt.str();

        if (bitsPerSample == 8)
            frames8.at(currIndex).save(tifPath.string());
        else
            frames16.at(currIndex).save(tifPath.string());
    }
}

unsigned int Movie::intLog10(unsigned int value) const
{
    // Log10 function that acts on integers and returns an integer.
    int power = 0;
    while (value != 0)
    {
        ++power;
        value /= 10;
    }
    return power;
}


void Movie::getFrameIntensityMinMax(size_t frameIndex,
                                    uint16_t& min, uint16_t& max) const
{
    if (bitsPerSample == 8)
    {
        uint8_t* pixelsData = frames8.at(frameIndex).pixelsData;
        min = *std::min_element(pixelsData, pixelsData + width * height);
        max = *std::max_element(pixelsData, pixelsData + width * height);
    }
    else
    {
        uint16_t* pixelsData = frames16.at(frameIndex).pixelsData;
        min = *std::min_element(pixelsData, pixelsData + width * height);
        max = *std::max_element(pixelsData, pixelsData + width * height);
    }
}

void Movie::getIntensityMinMax(uint16_t& min, uint16_t& max) const
{
    min = std::numeric_limits<uint16_t>::max();
    max = std::numeric_limits<uint16_t>::min();
    uint16_t currMin;
    uint16_t currMax;
    for (currIndex = 0; currIndex < nFrames; ++currIndex)
    {
        getFrameIntensityMinMax(currIndex, currMin, currMax);
        if (min > currMin) min = currMin;
        if (max < currMax) max = currMax;
    }
}


uint8_t* Movie::frameData8(const size_t i,
                           const unsigned int customBitDepth) const
{
    size_t sz = width * height * sizeof(uint8_t);
    uint8_t* data = new uint8_t[sz];
    int bitsShift = customBitDepth - 8;
    if (bitsPerSample == 8 && customBitDepth == 8)
        std::memcpy(data, frames8.at(i).pixelsData, sz);
    else if (bitsPerSample == 8) // bitShift != 0
    {
        uint8_t* data8 = frames8.at(i).pixelsData;
        if (bitsShift > 0)
            for (unsigned int k = 0; k < width * height; k++)
                data[k] = data8[k] >> bitsShift;
        else // bitsShift < 0
            for (unsigned int k = 0; k < width * height; k++)
                data[k] = data8[k] << -bitsShift;
    }
    else // bitsPerSample = 16
    {
        uint16_t* data16 = frames16.at(i).pixelsData;
        if (bitsShift == 0)
            for (unsigned int k = 0; k < width * height; k++)
                data[k] = (uint8_t) (data16[k]);
        else if (bitsShift > 0)
            for (unsigned int k = 0; k < width * height; k++)
                data[k] = (uint8_t) (data16[k] >> bitsShift);
        else // bitsShift < 0
            for (unsigned int k = 0; k < width * height; k++)
                data[k] = (uint8_t) (data16[k] << -bitsShift);
    }
    return data;
}

uint8_t* Movie::frameData8(const size_t i,
                           const unsigned int minValue,
                           const unsigned int maxValue) const
{
    size_t sz = width * height * sizeof(uint8_t);
    uint8_t* data = new uint8_t[sz];
    if (bitsPerSample == 8)
    {
        uint8_t* data8 = frames8.at(i).pixelsData;
        uint8_t amplitude = maxValue - minValue;
        for (unsigned int k = 0; k < width * height; k++)
        {
            // Data type needs to hold 255 * uint8_t. Therefore, use uint16_t.
            uint16_t val = (uint16_t) 255 * (data8[k] - minValue) / amplitude;
            data[k] = (uint8_t) (val);
        }
    }
    else // bitsPerSample = 16
    {
        uint16_t* data16 = frames16.at(i).pixelsData;
        uint16_t amplitude = maxValue - minValue;
        for (unsigned int k = 0; k < width * height; k++)
        {
            // Data type needs to hold 255 * uint16_t. Therefore, use uint32_t.
            uint32_t val = (uint32_t) 255 * (data16[k] - minValue) / amplitude;
            data[k] = (uint8_t) (val);
        }
    }
    return data;
}

MovieFormats::PixelFmt Movie::safeStrToPixelFmt(const std::string pixelFmtStr) const
{
    MovieFormats::PixelFmt pixelFmt;
    try
    {
        pixelFmt = MovieFormats::StrToPixelFmt.at(pixelFmtStr);
    }
    catch (std::out_of_range)
    {
        throw MovieException("Invalid pixel format string.");
    }
    return pixelFmt;
}

MovieFormats::PixelFmt Movie::safeInt32ToPixelFmt(const uint32_t pixelFmtInt) const
{
    MovieFormats::PixelFmt pixelFmt;
    try
    {
        pixelFmt = MovieFormats::Int32ToPixelFmt.at(pixelFmtInt);
    }
    catch (std::out_of_range)
    {
        throw MovieException("Invalid pixel format uint32 value.");
    }
    return pixelFmt;
}
