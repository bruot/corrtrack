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


#include <map>
#include <string>


namespace MovieFormats
{
    // See http://isgcameras.com/wp-content/uploads/2015/09/AIA_Pixel_Format_Naming_Convention_1-1-00.pdf

    enum class PixelFmt {
        Mono8,
        Mono10,
        Mono12,
        Mono14,
        Mono16,
    };

    enum class Endianness {
        little,
        big,
    };

    static std::map<std::string, PixelFmt> createStrToPixelFmtMap()
    {
        std::map<std::string, PixelFmt> m;
        m["Mono8"] = PixelFmt::Mono8;
        m["Mono10"] = PixelFmt::Mono10;
        m["Mono12"] = PixelFmt::Mono12;
        m["Mono14"] = PixelFmt::Mono14;
        m["Mono16"] = PixelFmt::Mono16;

        return m;
    }
    static const std::map<std::string, PixelFmt> StrToPixelFmt = createStrToPixelFmtMap();

    static std::map<uint32_t, PixelFmt> createInt32ToPixelFmtMap()
    {
        std::map<uint32_t, PixelFmt> m;
        m[0x01080001] = PixelFmt::Mono8;
        m[0x01100003] = PixelFmt::Mono10;
        m[0x01100005] = PixelFmt::Mono12;
        m[0x01100025] = PixelFmt::Mono14;
        m[0x01100007] = PixelFmt::Mono16;

        return m;
    }
    static const std::map<uint32_t, PixelFmt> Int32ToPixelFmt = createInt32ToPixelFmtMap();

    static std::map<PixelFmt, uint32_t> createPixelFmtToInt32Map()
    {
        std::map<PixelFmt, uint32_t> m;
        m[PixelFmt::Mono8] = 0x01080001;
        m[PixelFmt::Mono10] = 0x01100003;
        m[PixelFmt::Mono12] = 0x01100005;
        m[PixelFmt::Mono14] = 0x01100025;
        m[PixelFmt::Mono16] = 0x01100007;

        return m;
    }
    static const std::map<PixelFmt, uint32_t> PixelFmtToInt32 = createPixelFmtToInt32Map();

    static std::map<PixelFmt, unsigned int> createPixelFmtBitsPerSampleMap()
    {
        std::map<PixelFmt, unsigned int> m;
        m[PixelFmt::Mono8] = 8;
        m[PixelFmt::Mono10] = 16;
        m[PixelFmt::Mono12] = 16;
        m[PixelFmt::Mono14] = 16;
        m[PixelFmt::Mono16] = 16;

        return m;
    }
    static const std::map<PixelFmt, unsigned int> PixelFmtBitsPerSample = createPixelFmtBitsPerSampleMap();

    static std::map<PixelFmt, unsigned int> createPixelFmtBitDepthMap()
    {
        std::map<PixelFmt, unsigned int> m;
        m[PixelFmt::Mono8] = 8;
        m[PixelFmt::Mono10] = 10;
        m[PixelFmt::Mono12] = 12;
        m[PixelFmt::Mono14] = 14;
        m[PixelFmt::Mono16] = 16;

        return m;
    }
    static const std::map<PixelFmt, unsigned int> PixelFmtBitDepth = createPixelFmtBitDepthMap();

    static std::map<PixelFmt, uint16_t> createPixelFmt16BitsMasksMap()
    {
        std::map<PixelFmt, uint16_t> m;
        m[PixelFmt::Mono10] = 0x03ff;
        m[PixelFmt::Mono12] = 0x0fff;
        m[PixelFmt::Mono14] = 0x3fff;
        m[PixelFmt::Mono16] = 0xffff;

        return m;
    }
    static const std::map<PixelFmt, uint16_t> PixelFmt16BitsMasks = createPixelFmt16BitsMasksMap();
}
