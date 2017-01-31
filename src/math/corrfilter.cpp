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
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "corrfilter.h"
#include "io/exceptions/ioexception.h"


const char* CorrFilter::CorrFilterFormatException::what() const noexcept
{
    return "Filter file format error.";
}

CorrFilter::CorrFilter()
    : filter{nullptr}
{}

void CorrFilter::setFilter(const std::string fileName)
{
    std::vector<std::string> elems;
    filterFileName = fileName;
    try
    {
        std::ifstream file;
        file.exceptions(std::ios::failbit | std::ios::badbit);
        file.open(fileName.c_str());
        file.exceptions(std::ios::badbit);
        std::string line;
        bool first = true;
        while (std::getline(file, line))
        {
            std::vector<std::string> columns;
            boost::algorithm::split(columns, line, boost::is_any_of("\t"));
            if (first)
            {
                width = (unsigned int) (columns.size());
                first = false;
            }
            // Append columns to elems
            elems.reserve(elems.size() + columns.size()); // preallocate memory
            elems.insert(elems.end(), columns.begin(), columns.end());
        }
        height = (unsigned int) (elems.size()) / width;
        file.close();
    }
    catch (const std::ios_base::failure)
    {
        throw IOException();
    }

    // Build correlation filter array from elems
    try
    {
        filter = new double[width * height];
        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int x = 0; x < width; x++)
            {
                unsigned int i = width * y + x;
                filter[i] = stod(elems[i]);
            }
        }
    }
    catch (std::invalid_argument)
    {
        throw CorrFilterFormatException();
    }
}

bool CorrFilter::isFilterSet() const
{
    return (filter != nullptr);
}

CorrFilter::~CorrFilter()
{
    delete[] filter;
}

double CorrFilter::getFilterValue(const unsigned int x,
                                  const unsigned int y) const
{
    unsigned int i = width * y + x;
    return filter[i];
}

