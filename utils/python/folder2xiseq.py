#!/usr/bin/env python
# -*- coding: utf-8 *-*

# This file is part of the particle tracking software CorrTrack.
#
# Copyright 2016, 2017 Nicolas Bruot
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


"""Creates a .xiseq file to make a movie from a folder of images."""


import os
import sys
import corrtrack.conversions as conversions


if __name__ == '__main__':
    args = sys.argv
    if len(args) < 3:
        sys.stderr.write('usage: folder2xiseq.py PIXEL_FORMAT FOLDER_1 [FOLDER_2 ...]\n')
        sys.exit(2)
    pixel_format = args[1]
    for arg in args[2:]:
        if os.path.isdir(arg):
            folder = os.path.abspath(arg)
            xi_path = '%s.xiseq' % folder
            if os.path.exists(xi_path):
                sys.stderr.write('Output .xiseq file already exists.\n')
                sys.exit(1)
            conversions.folder2xiseq(arg, pixel_format)
        else:
            sys.stderr.write("Warning: Not a directory: %s.\n" % arg)
