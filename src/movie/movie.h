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


#pragma once


#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include "base/frame.h"
#include "base/movieformats.h"


/*
 * Structures for Cine files
 */

#define CINE_OLDMAXFILENAME 65
#define CINE_MAXLENDESCRIPTION_OLD 121
#define CINE_MAXLENDESCRIPTION 4096

#if !defined(_WBGAIN_)
#define CINE__WBGAIN_
    typedef struct CINE_tagWBGAIN
    {
        float R;
        float B;
    }
    CINE_WBGAIN, *CINE_PWBGAIN;
#endif

#if !defined(_WINDOWS)
typedef struct CINE_tagRECT
{
    int32_t left;
    int32_t top;
    int32_t right;
    int32_t bottom;
} CINE_RECT, *CINE_PRECT;
#endif

typedef int cine_bool32_t;

typedef uint32_t CINE_FRACTIONS, *CINE_PFRACTIONS;

typedef struct CINE_tagTIME64
{
    CINE_FRACTIONS fractions;
    uint32_t seconds;
} CINE_TIME64, *CINE_PTIME64;

typedef struct CINE_tagIMFILTER
{
    int32_t dim;
    int32_t shifts;
    int32_t bias;
    int32_t Coef[5*5];
}
CINE_IMFILTER, *CINE_PIMFILTER;

typedef struct CINE_tagTC
{
    uint8_t framesU:4;
    uint8_t framesT:2;
    uint8_t dropFrameFlag:1;
    uint8_t colorFrameFlag:1;
    uint8_t secondsU:4;
    uint8_t secondsT:3;
    uint8_t flag1:1;
    uint8_t minutesU:4;
    uint8_t minutesT:3;
    uint8_t flag2:1;
    uint8_t hoursU:4;
    uint8_t hoursT:2;
    uint8_t flag3:1;
    uint8_t flag4:1;
    uint32_t userBitData;
}
CINE_TC, *CINE_PTC;

typedef struct CINE_tagCINEFILEHEADER
{
    uint16_t Type;
    uint16_t Headersize;
    uint16_t Compression;
    uint16_t Version;
    int32_t FirstMovieImage;
    uint32_t TotalImageCount;
    int32_t FirstImageNo;
    uint32_t ImageCount;
    uint32_t OffImageHeader;
    uint32_t OffSetup;
    uint32_t OffImageOffsets;
    CINE_TIME64 TriggerTime;
} CINE_CINEFILEHEADER;

typedef struct CINE_tagBITMAPINFOHEADER
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} CINE_BITMAPINFOHEADER;

typedef struct CINE_tagSETUP
{
    uint16_t FrameRate16;
    uint16_t Shutter16;
    uint16_t PostTrigger16;
    uint16_t FrameDelay16;
    uint16_t AspectRatio;
    uint16_t Res7;
    uint16_t Res8;
    uint8_t Res9;
    uint8_t Res10;
    uint8_t Res11;
    uint8_t TrigFrame;
    uint8_t Res12;
    char DescriptionOld[CINE_MAXLENDESCRIPTION_OLD];
    uint16_t Mark;
    uint16_t Length;
    uint16_t Res13;
    uint16_t SigOption;
    int16_t BinChannels;
    uint8_t SamplesPerImage;
    char BinName[8][11];
    uint16_t AnaOption;
    int16_t AnaChannels;
    uint8_t Res6;
    uint8_t AnaBoard;
    int16_t ChOption[8];
    float AnaGain[8];
    char AnaUnit[8][6];
    char AnaName[8][11];
    int32_t lFirstImage;
    uint32_t dwImageCount;
    int16_t nQFactor;
    uint16_t wCineFileType;
    char szCinePath[4][CINE_OLDMAXFILENAME];
    uint16_t Res14;
    uint8_t Res15;
    uint8_t Res16;
    uint16_t Res17;
    double Res18;
    double Res19;
    uint16_t Res20;
    int32_t Res1;
    int32_t Res2;
    int32_t Res3;
    uint16_t ImWidth;
    uint16_t ImHeight;
    uint16_t EDRShutter16;
    uint32_t Serial;
    int32_t Saturation;
    uint8_t Res5;
    uint32_t AutoExposure;
    cine_bool32_t bFlipH;
    cine_bool32_t bFlipV;
    uint32_t Grid;
    uint32_t FrameRate;
    uint32_t Shutter;
    uint32_t EDRShutter;
    uint32_t PostTrigger;
    uint32_t FrameDelay;
    cine_bool32_t bEnableColor;
    uint32_t CameraVersion;
    uint32_t FirmwareVersion;
    uint32_t SoftwareVersion;
    int32_t RecordingTimeZone;
    uint32_t CFA;
    int32_t Bright;
    int32_t Contrast;
    int32_t Gamma;
    uint32_t Res21;
    uint32_t AutoExpLevel;
    uint32_t AutoExpSpeed;
    CINE_RECT AutoExpRect;
    CINE_WBGAIN WBGain[4];
    int32_t Rotate;
    CINE_WBGAIN WBView;
    uint32_t RealBPP;
    uint32_t Conv8Min;
    uint32_t Conv8Max;
    int32_t FilterCode;
    int32_t FilterParam;
    CINE_IMFILTER UF;
    uint32_t BlackCalSVer;
    uint32_t WhiteCalSVer;
    uint32_t GrayCalSVer;
    cine_bool32_t bStampTime;
    uint32_t SoundDest;
    uint32_t FRPSteps;
    int32_t  FRPImgNr[16];
    uint32_t FRPRate[16];
    uint32_t FRPExp[16];
    int32_t MCCnt;
    float MCPercent[64];
    uint32_t CICalib;
    uint32_t CalibWidth;
    uint32_t CalibHeight;
    uint32_t CalibRate;
    uint32_t CalibExp;
    uint32_t CalibEDR;
    uint32_t CalibTemp;
    uint32_t HeadSerial[4];
    uint32_t RangeCode;
    uint32_t RangeSize;
    uint32_t Decimation;
    uint32_t MasterSerial;
    uint32_t Sensor;
    uint32_t ShutterNs;
    uint32_t EDRShutterNs;
    uint32_t FrameDelayNs;
    uint32_t ImPosXAcq;
    uint32_t ImPosYAcq;
    uint32_t ImWidthAcq;
    uint32_t ImHeightAcq;
    char Description[CINE_MAXLENDESCRIPTION];
    cine_bool32_t RisingEdge;
    uint32_t FilterTime;
    cine_bool32_t LongReady;
    cine_bool32_t ShutterOff;
    uint8_t Res4[16];
    cine_bool32_t bMetaWB;
    int32_t Hue;
    int32_t BlackLevel;
    int32_t WhiteLevel;
    char  LensDescription[256];
    float LensAperture;
    float LensFocusDistance;
    float LensFocalLength;
    float fOffset;
    float fGain;
    float fSaturation;
    float fHue;
    float fGamma;
    float fGammaR;
    float fGammaB;
    float fFlare;
    float fPedestalR;
    float fPedestalG;
    float fPedestalB;
    float fChroma;
    char  ToneLabel[256];
    int32_t TonePoints;
    float fTone[32*2];
    char  UserMatrixLabel[256];
    cine_bool32_t  EnableMatrices;
    float fUserMatrix[9];
    cine_bool32_t  EnableCrop;
    CINE_RECT  CropRect;
    cine_bool32_t  EnableResample;
    uint32_t  ResampleWidth;
    uint32_t  ResampleHeight;
    float fGain16_8;
    uint32_t  FRPShape[16];
    CINE_TC    TrigTC;
    float fPbRate;
    float fTcRate;
    char CineName[256];
} CINE_SETUP;

/*
 * End of Structures for Cine files
 */


class Movie
{
private:
    void deleteAllFrames();
    void loadRawmMovie();
    void loadXiseqMovie();
    void loadPdsMovie();
    void loadCineMovie();
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
    double time64DiffToDouble(CINE_TIME64 time1, CINE_TIME64 time2);
    MovieFormats::PixelFmt safeStrToPixelFmt(const std::string pixelFmtStr) const;
    MovieFormats::PixelFmt safeInt32ToPixelFmt(const uint32_t pixelFmtInt) const;

    struct frameMetadata
    {
        // This is used by all movie formats, but the fileName field may be
        // unused.
        uint64_t timestamp;
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
        Pds,
        Cine,
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
    std::vector<uint64_t> timestamps;
    mutable size_t currIndex;
};
