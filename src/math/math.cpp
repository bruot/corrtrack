/*
 * This file is part of the particle tracking software CorrTrack.
 *
 * Copyright 2017 Nicolas Bruot
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


#include <stdexcept>
#include "math/math.h"


int math::ipow(int base, int exp)
{
    int result = 1;
    if (exp >= 0)
    {
        while (exp)
        {
            if (exp & 1)
                result *= base;
            exp >>= 1;
            base *= base;
        }
    }
    else
    {
        result = 0;
    }

    return result;
}

long math::ilog2(long value)
{
    if (value <= 0)
        throw std::invalid_argument("received negative value");
    unsigned int power = 0;
    while (value >>= 1) ++power;
    return power;
}
