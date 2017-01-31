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


#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>


class Version
{
private:
    // An internal utility structure just used to make the std::copy in the
    // constructor easy to write.
    struct VersionDigit
    {
        int value;
        operator int() const;
    };

    friend std::istream& operator >>(std::istream& str,
                                     Version::VersionDigit& digit);

    std::vector<int> versionInfo;

public:
    Version(std::string const& versionStr);
    bool operator <(Version const& rhs) const;
};

