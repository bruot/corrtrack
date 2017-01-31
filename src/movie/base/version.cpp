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
#include <sstream>
#include <vector>
#include <iterator>

#include "version.h"


Version::VersionDigit::operator int() const
{
    return value;
}

Version::Version(std::string const& versionStr)
{
    // To make processing easier in VersionDigit prepend a '.'
    std::stringstream versionStream(std::string(".") + versionStr);

    // Copy all parts of the version number into the version Info vector.
    std::copy(std::istream_iterator<VersionDigit>(versionStream),
              std::istream_iterator<VersionDigit>(),
              std::back_inserter(versionInfo)
              );
}

bool Version::operator <(Version const& rhs) const
{
    return std::lexicographical_compare(versionInfo.begin(),
                                        versionInfo.end(),
                                        rhs.versionInfo.begin(),
                                        rhs.versionInfo.end());
}

std::istream& operator >>(std::istream& str, Version::VersionDigit& digit)
{
    // Read a single digit from the version.
    str.get();
    str >> digit.value;
    return str;
}
