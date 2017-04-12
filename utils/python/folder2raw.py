#!/usr/bin/env python
# -*- coding: utf-8 *-*

# This file is part of the particle tracking software CorrTrack.
#
# Copyright 2017 Nicolas Bruot
#
#
# CorrTrack is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# CorrTrack is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with CorrTrack.  If not, see <http://www.gnu.org/licenses/>.


"""Converts a folder of images to a .raw/.rawm movie."""


import os
import sys
import corrtrack.conversions as conversions


if __name__ == '__main__':
    args = sys.argv
    if len(args) < 3:
        sys.stderr.write('usage: folder2raw.py PIXEL_FORMAT FOLDER_1 [FOLDER_2 ...]\n')
        exit(2)
    pixel_format = args[1]
    for arg in args[2:]:
        if os.path.isdir(arg):
            conversions.folder2raw(arg, pixel_format)
        else:
            sys.stderr.write("Warning: Not a directory: %s.\n" % arg)
