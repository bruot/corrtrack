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


#ifdef _WIN32
    #include <tiff/tiff.h>
    #include <tiff/tiffio.h>
#else
    #include <tiff.h>
    #include <tiffio.h>
#endif

#include <QImage>
#include <algorithm>
#include <boost/filesystem/path.hpp>
#include "movieformats.h"
#include "frame.h"


namespace fs = boost::filesystem;


template <typename PixelDataType>
const char* Frame<PixelDataType>::FrameLoadException::what() const noexcept
{
    return "Frame load error.";
}

template <typename PixelDataType>
const char* Frame<PixelDataType>::FrameSaveException::what() const noexcept
{
    return "Frame save error.";
}

template <typename PixelDataType>
Frame<PixelDataType>::Frame()
    : timestamp{0}, width{NULL}, height{NULL}, pixelsData{NULL}
{}

template <typename PixelDataType>
void Frame<PixelDataType>::load(const std::string fileName,
                                const MovieFormats::PixelFmt pixelFmt,
                                const unsigned long timestamp)
{
    this->timestamp = timestamp;

    fs::path path(fileName);
    fs::path ext = path.extension();

    if (ext == ".tif" || ext == ".tiff")
    {
        // Use libtiff that supports 8 and 16 bits images.
        TIFF *tif = TIFFOpen(fileName.c_str(), "r");
        if (tif)
        {
            TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
            TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
            uint16 bitsPerSample;
            TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bitsPerSample);
            uint16 samplesPerPixel;
            TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel);
            tdata_t buf;
            buf = _TIFFmalloc(TIFFScanlineSize(tif));
            pixelsData = new PixelDataType[width * height];

            if (bitsPerSample != MovieFormats::PixelFmtBitsPerSample.at(pixelFmt))
                throw FrameLoadException();

            // This does not support "tiles" images.
            if (bitsPerSample > 8)
            {
                uint16_t mask = MovieFormats::PixelFmt16BitsMasks.at(pixelFmt);

                for(unsigned int i = 0; i < height; i++)
                {
                    TIFFReadScanline(tif, buf, i);
                    for (unsigned int j = 0; j < width; j++)
                    {
                        uint16 pixelValue = ((uint16*) buf)[j];
                        pixelsData[i * width + j] = (PixelDataType) (pixelValue & mask);
                    }
                }
            }
            else
            {
                // Import image
                for(unsigned int i = 0; i < height; i++)
                {
                    TIFFReadScanline(tif, buf, i);
                    std::memcpy(pixelsData + i * width,
                                (PixelDataType *) buf,
                                width * sizeof(PixelDataType));
                }
            }
            _TIFFfree(buf);
            TIFFClose(tif);
        }
        else
        {
            throw FrameLoadException();
        }
    }
    else
    {
        // Use QImage that can load many image formats.
        QImage *qi = new QImage();
        try
        {
            qi->load(QString(fileName.c_str()));
        }
        catch (std::exception)
        {
            throw FrameLoadException();
        }
        width = qi->width();
        height = qi->height();
        pixelsData = new PixelDataType[width * height];
        const unsigned int size = width * height * sizeof(PixelDataType);
        std::copy(qi->bits(), qi->bits() + size, pixelsData);
        delete qi;
    }
}

template <typename PixelDataType>
void Frame<PixelDataType>::load(const PixelDataType * const pixelsData,
                                const uint32_t width, const uint32_t height,
                                const unsigned long timestamp)
{
    this->width = width;
    this->height = height;
    this->timestamp = timestamp;
    this->pixelsData = new PixelDataType[width * height]();

    size_t nPixels = width * height;
    std::copy(pixelsData,
              pixelsData + nPixels,
              this->pixelsData);
}

template <typename PixelDataType>
Frame<PixelDataType>::~Frame()
{
    delete[] pixelsData;
}

template <typename PixelDataType>
PixelDataType Frame<PixelDataType>::getPixelIntensity(const unsigned int x,
                                                      const unsigned int y) const
{
    // First pixel is (0, 0).

    unsigned int i = width * y + x;
    return pixelsData[i];
}

template <typename PixelDataType>
void Frame<PixelDataType>::save(const std::string fileName) const
{
    TIFF *outputImg;
    if((outputImg = TIFFOpen(fileName.c_str(), "w")) == NULL)
          throw FrameSaveException();

    TIFFSetField(outputImg, TIFFTAG_IMAGEWIDTH, (uint16) width);
    TIFFSetField(outputImg, TIFFTAG_IMAGELENGTH, (uint16) height);
    TIFFSetField(outputImg, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(outputImg, TIFFTAG_BITSPERSAMPLE, 8 * sizeof(PixelDataType));
    TIFFSetField(outputImg, TIFFTAG_ROWSPERSTRIP, height);
    TIFFSetField(outputImg, TIFFTAG_ORIENTATION, (int) ORIENTATION_TOPLEFT);
    TIFFSetField(outputImg, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(outputImg, TIFFTAG_COMPRESSION, COMPRESSION_NONE);
    TIFFSetField(outputImg, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

    // Write the data to the file
     tsize_t imageSize = TIFFWriteEncodedStrip(outputImg, 0, pixelsData,
                                               width * height * sizeof(PixelDataType));
     if(imageSize == -1)
        throw FrameSaveException();

    TIFFWriteDirectory(outputImg);
    TIFFClose(outputImg);
}
