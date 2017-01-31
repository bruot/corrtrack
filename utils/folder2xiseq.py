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


"""
Creates a .xiseq file to make a movie from a folder of images.
"""


import os
import sys
from natsort import natsorted


PIXEL_FORMAT_VALUES = {
                 'mono8': 0x01080001,
                 'mono10': 0x01100003,
                 'mono12': 0x01100005,
                 'mono14': 0x01100025,
                 'mono16': 0x01100007,
                }

IMAGE_EXTENSIONS = (
                    '.bmp',
                    '.tiff',
                    '.tif',
                    '.png',
                    '.jpg',
                    '.jpeg',
                   )


def write_xiseq_header(f, pixel_format):
    f.write('<ImageSequence version="4.8.2">\n')
    f.write(' <imageMetadata>\n')
    val = PIXEL_FORMAT_VALUES[pixel_format.lower()]
    f.write('  <apiContextList>xiApiImg:format=%d\n' % val)
    f.write('  </apiContextList>\n')
    f.write(' </imageMetadata>\n')


def write_xiseq_footer(f):
    f.write('</ImageSequence>\n')


def make_xiseq_file(folder, pixel_format):
    """
    Builds a .xiseq file from the given folder.
    """

    folder = folder.rstrip('/').rstrip('\\')
    xi_path = '%s.xiseq' % folder
    base_folder = os.path.split(folder)[1]
    with open(xi_path, 'w') as f:
        write_xiseq_header(f, pixel_format)

        paths = os.listdir(folder)
        paths = natsorted(paths)
        timestamp = 0
        for path in paths:
            relpath = os.path.join(base_folder, path)
            if relpath.endswith(IMAGE_EXTENSIONS):
                f.write(' <file timestamp="%d">%s</file>\n' % (timestamp,
                                                               relpath))
            timestamp += 1
        write_xiseq_footer(f)


if __name__ == '__main__':
    args = sys.argv
    if len(args) < 3:
        sys.stderr.write('usage: folder2xiseq.py PIXEL_FORMAT FOLDER_1 [FOLDER_2 ...]\n')
        exit(2)
    pixel_format = args[1]
    for arg in args[2:]:
        if os.path.isdir(arg):
            make_xiseq_file(arg, pixel_format)
        else:
            sys.stderr.write("Warning: Not a directory: %s.\n" % arg)
