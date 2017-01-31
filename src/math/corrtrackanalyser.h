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


#include <exception>
#include "corrfilter.h"
#include "movie/movie.h"
#include "movie/base/frame.h"
#include "point.h"
#include "imaged.h"
#include "pointd.h"


class CorrTrackAnalyser
{
private:
    double correlationValue(const unsigned int i0, const unsigned int j0) const;
    ImageD* calcCorrelationMap(const Point point) const;
    PointD subPixelRes(const ImageD * const correlationMap) const;
    void copyFilter() const;

    // filterData and currImageData allow for faster access than filter->filter,
    // and than using an ImageD.
    mutable double *filterData;
    mutable unsigned int filterWidth;
    mutable unsigned int filterHeight;
    //
    double *currImageData;
    unsigned int currImageWidth;
    unsigned int currImageHeight;
    //
    std::vector<Point> *pointsList;

public:
    CorrTrackAnalyser();
    ~CorrTrackAnalyser();
    CorrTrackAnalyser(const CorrTrackAnalyser&) =delete;
    CorrTrackAnalyser& operator=(const CorrTrackAnalyser&) =delete;
    CorrTrackAnalyser(CorrTrackAnalyser&&) =delete;
    CorrTrackAnalyser& operator=(CorrTrackAnalyser&&) =delete;

    void setMovie(std::string fileName);
    void resetMovie();
    void addPoint(Point point);
    void removeLastPoint();
    void clearPoints();
    void selectImage(size_t frameIndex);
    void analyse();
    bool isFilterSet() const;
    std::vector<ImageD*>* testCorrelation();
    std::vector<Point>* getPoints() const;

    CorrFilter *filter;
    Movie *movie;
    unsigned int windowWidth;
    unsigned int windowHeight;
    double fitRadius;
    size_t currFrameIndex;

    class AnalyseException : public std::exception
    {
    private:
        std::string _message;
    public:
        explicit AnalyseException(const std::string message);
        virtual const char* what() const noexcept override;
    };
};
