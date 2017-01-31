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


class CorrFilter
{
public:
    CorrFilter();
    ~CorrFilter();
    CorrFilter(const CorrFilter&) =delete;
    CorrFilter& operator=(const CorrFilter&) =delete;
    CorrFilter(CorrFilter&&) =delete;
    CorrFilter& operator=(CorrFilter&&) =delete;

    void setFilter(const std::string fileName);
    bool isFilterSet() const;
    double getFilterValue(const unsigned int x, const unsigned int y) const;

    std::string filterFileName;
    unsigned int width;
    unsigned int height;
    double *filter;

    class CorrFilterFormatException : public std::exception
    {
    public:
        virtual const char* what() const noexcept override;
    };
};
