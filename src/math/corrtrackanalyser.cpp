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


#include <iostream>
#include <iomanip>
#include <fstream>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include "constants.h"
#include "corrtrackanalyser.h"
#include "movie/movie.h"
#include "math/corrfilter.h"
#include "math/imaged.h"


CorrTrackAnalyser::CorrTrackAnalyser()
    : filterData{nullptr},
      filterWidth{0}, filterHeight{0},
      currImageData{nullptr},
      currImageWidth{0}, currImageHeight{0},
      pointsList{new std::vector<Point>()},
      filter{new CorrFilter()},
      movie{new Movie()},
      windowWidth{15}, windowHeight{15},
      fitRadius{1.5},
      currFrameIndex{0}
{}

CorrTrackAnalyser::~CorrTrackAnalyser()
{
    delete movie;
    if (filterData != nullptr) delete filterData;
    if (currImageData != nullptr) delete currImageData;

    delete filter;
    delete pointsList;
}

CorrTrackAnalyser::AnalyseException::AnalyseException(const std::string message) :
    std::exception()
{
    _message = message;
}

const char* CorrTrackAnalyser::AnalyseException::what() const noexcept
{
    return _message.c_str();
}

void CorrTrackAnalyser::setMovie(std::string fileName)
{
    movie->openMovie(fileName);
}

void CorrTrackAnalyser::resetMovie()
{
    // Resets the movie to an empty movie
    delete movie;
    movie = new Movie();
}

void CorrTrackAnalyser::addPoint(Point point)
{
    pointsList->push_back(point);
}

void CorrTrackAnalyser::removeLastPoint()
{
    pointsList->pop_back();
}

void CorrTrackAnalyser::clearPoints()
{
    pointsList->clear();
}

std::vector<Point>* CorrTrackAnalyser::getPoints() const
{
    return pointsList;
}

double CorrTrackAnalyser::correlationValue(const unsigned int i0,
                                           const unsigned int j0) const
{
    // The desired frame must be loaded in currImage and currFrameIndex.
    double correlation = 0.0;
    const unsigned int iStart = i0 - (int)(filterWidth / 2);
    const unsigned int jStart = j0 - (int)(filterHeight / 2);
    for (unsigned int j = 0; j < filterHeight; j++)
    {
        for (unsigned int i = 0; i < filterWidth; i++)
        {
            correlation += currImageData[(jStart + j) * currImageWidth + iStart + i]
                           * filterData[j * filterWidth + i];
        }
    }
    return correlation;
}

void CorrTrackAnalyser::selectImage(size_t frameIndex)
{
    // Switch to desired frame.

    if (currImageData != nullptr) delete currImageData;

    currFrameIndex = frameIndex;
    currImageWidth = movie->width;
    currImageHeight = movie->height;
    currImageData = new double[currImageWidth * currImageHeight];

    if (movie->bitsPerSample == 8)
    {
        uint8_t* data = movie->frames8.at(frameIndex).pixelsData;
        for (unsigned int k = 0; k < currImageWidth * currImageHeight; k++)
            currImageData[k] = (double) data[k];
    }
    else
    {
        uint16_t* data = movie->frames16.at(frameIndex).pixelsData;
        for (unsigned int k = 0; k < currImageWidth * currImageHeight; k++)
            currImageData[k] = (double) data[k];
    }
}

ImageD* CorrTrackAnalyser::calcCorrelationMap(const Point point) const
{
    ImageD *correlationMap = new ImageD(windowWidth, windowHeight);
    const int iStart = point.x - (int)(windowWidth / 2);
    const int jStart = point.y - (int)(windowHeight / 2);
    // Check boundaries
    const int iMin = iStart - (int)(filterWidth / 2);
    const int jMin = jStart - (int)(filterHeight / 2);
    const int iMax = iMin + (windowWidth - 1) + (filterWidth - 1);
    const int jMax = jMin + (windowHeight - 1) + (filterHeight - 1);
    if (iMin < 0 || iMax >= (int)(currImageWidth)
            || jMin < 0 || jMax >= (int)(currImageHeight))
    {
        std::string message("Correlation window out of image boundaries.");
        throw AnalyseException(message);
    }
    // Calculate correlation
    for (unsigned int i = 0; i < windowWidth; i++)
    {
        for (unsigned int j = 0; j < windowHeight; j++)
        {
            const double correlation = correlationValue(iStart + i, jStart + j);
            correlationMap->setPixelIntensity(i, j, correlation);
        }
    }
    return correlationMap;
}

std::vector<ImageD*>* CorrTrackAnalyser::testCorrelation()
{
    // Returns the result of the correlation filter on the given frame.
    //
    // The frame must already be selected.

    // TODO: possibly dangling pointer returned

    copyFilter();

    std::vector<ImageD*> *correlationMaps = new std::vector<ImageD*>;
    ImageD *correlationMap;
    for (Point const& point: *pointsList)
    {
        correlationMap = calcCorrelationMap(point);
        correlationMaps->push_back(correlationMap);
    }
    return correlationMaps;
}

void CorrTrackAnalyser::analyse()
{
    // Initialize parameters
    ImageD* correlationMap;
    double x, y;
    std::vector<Point> *movingPointsList = new std::vector<Point>;
    *movingPointsList = *pointsList;
    boost::filesystem::path path(movie->fileName);
    path = boost::filesystem::change_extension(path, "dat");
    std::string outputFileName = path.string();
    std::ofstream outputFile(outputFileName);

    // This is set before the big loops that need to be efficient.
    copyFilter();

    if (outputFile.is_open())
    {
        // Print header
        outputFile << "# " << constants::APP_NAME << " ";
        if (std::strcmp(constants::VERSION, constants::TARGET_VERSION) == 0)
            outputFile << "version " << constants::VERSION;
        else
            outputFile << "development version " << constants::VERSION
                       << "->" << constants::TARGET_VERSION;
        outputFile << "\n";
        outputFile << "# Filter " << filter->filterFileName << "\n";
        outputFile << "# with window size (" << windowWidth
                   << ", " << windowHeight << ") and fit radius "
                   << fitRadius << ".\n";
        outputFile << "#\n";
        outputFile << "# Frame\tTimestamp";
        for (unsigned int k = 0; k < movingPointsList->size(); k++)
        {
            outputFile << "\tx_" << k + 1 << "\ty_" << k + 1;
        }
        outputFile << "\n";
        for (unsigned int i = 0; i < movie->nFrames; i++)
        {
            selectImage(i);
            outputFile << i + 1 << "\t" << movie->timestamps.at(i);
            for (Point &point : *movingPointsList)
            {
                correlationMap = calcCorrelationMap(point);
                PointD newPoint = subPixelRes(correlationMap);

                delete correlationMap;

                x = point.x - (windowWidth / 2) + newPoint.x;
                y = point.y - (windowHeight / 2) + newPoint.y;

                // The "+ 1.0" are because the first pixel is (0, 0) in this
                // program, while the usual convention is that the first pixel
                // is (1, 1).
                outputFile << std::fixed << std::setprecision(6)
                           << "\t" << x + 1.0
                           << "\t" << y + 1.0;

                point.setPos((unsigned int) (x + 0.5),
                             (unsigned int) (y + 0.5));
            }
            outputFile << "\n";
        }
        outputFile.close();
    }
}

PointD CorrTrackAnalyser::subPixelRes(const ImageD * const correlationMap) const
{
    const size_t shift = correlationMap->width * correlationMap->height;

    // Position of maximum correlation
    const size_t k = std::distance(correlationMap->pixelsData,
                                   std::max_element(correlationMap->pixelsData,
                                                    correlationMap->pixelsData + shift));
    const unsigned int jMax = (unsigned int) (k) / correlationMap->width;
    const unsigned int iMax = (unsigned int) (k) - jMax * correlationMap->width;

    const double fitRadius2 = fitRadius * fitRadius;
    const double shift_x = (double) iMax;
    const double shift_y = (double) jMax;

    std::vector<double> xVector, yVector, corrVector;

    for (unsigned int i = 0; i < correlationMap->width; i++)
    {
        for (unsigned int j = 0; j < correlationMap->height; j++)
        {
            const double x = (double) i;
            const double y = (double) j;
            const double radius2 = (x - shift_x) * (x - shift_x)
                                   + (y - shift_y) * (y - shift_y);
            if (radius2 <= fitRadius2)
            {
                xVector.push_back(x - shift_x);
                yVector.push_back(y - shift_y);
                corrVector.push_back(correlationMap->getPixelIntensity(i, j));
            }
        }
    }

    // Converts vectors to arrays.  It works because vector elements are stored
    // contiguously.
    double *x_data = &xVector[0];
    double *y_data = &yVector[0];
    double *corr_data = &corrVector[0];

    const size_t n = xVector.size();
    const int N_COEFFS = 6;

    if (n < N_COEFFS)
    {
        std::string message("Intersection between pixels within fit radius and correlation window only has ");
        message += std::to_string(n);
        message += " point(s), while at least ";
        message += std::to_string(N_COEFFS);
        message += " are required.";
        throw AnalyseException(message);
    }

    double chisq;

    gsl_vector *y, *coeffs;
    gsl_matrix *X, *cov;

    y = gsl_vector_alloc(n);
    coeffs = gsl_vector_alloc(N_COEFFS);
    X = gsl_matrix_alloc(n, N_COEFFS);
    cov = gsl_matrix_alloc(N_COEFFS, N_COEFFS);

    for (unsigned int i = 0; i < n; i++)
    {
        gsl_matrix_set(X, i, 0, x_data[i] * x_data[i]);
        gsl_matrix_set(X, i, 1, x_data[i] * y_data[i]);
        gsl_matrix_set(X, i, 2, y_data[i] * y_data[i]);
        gsl_matrix_set(X, i, 3, x_data[i]);
        gsl_matrix_set(X, i, 4, y_data[i]);
        gsl_matrix_set(X, i, 5, 1.0);

        gsl_vector_set(y, i, corr_data[i]);
    }

    gsl_multifit_linear_workspace *work = gsl_multifit_linear_alloc(n, N_COEFFS);
    gsl_multifit_linear(X, y, coeffs, cov, &chisq, work);
    gsl_multifit_linear_free(work);

    const double a = gsl_vector_get(coeffs, 0);
    const double b = gsl_vector_get(coeffs, 1);
    const double c = gsl_vector_get(coeffs, 2);
    const double d = gsl_vector_get(coeffs, 3);
    const double e = gsl_vector_get(coeffs, 4);
    // const double f = gsl_vector_get(coeffs, 5);

    gsl_vector_free(y);
    gsl_vector_free(coeffs);
    gsl_matrix_free(X);
    gsl_matrix_free(cov);

    // Position in correlationMap coordinates:
    double xPos, yPos;
    xPos = shift_x + (2*c*d - b*e) / (b*b - 4*a*c);
    yPos = shift_y + (2*a*e - b*d) / (b*b - 4*a*c);
    PointD point = PointD(xPos, yPos);

    return point;
}

void CorrTrackAnalyser::copyFilter() const
{
    filterData = filter->filter;
    filterWidth = filter->width;
    filterHeight = filter->height;
}
