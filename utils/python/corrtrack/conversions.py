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


"""Conversion tools for the CorrTrack software."""


import os
import sys
import numpy
import PIL.Image
from natsort import natsorted


PIXEL_FORMAT_VALUES = {
                       'mono8': 0x01080001,
                       'mono10': 0x01100003,
                       'mono12': 0x01100005,
                       'mono14': 0x01100025,
                       'mono16': 0x01100007,
                      }

PIXEL_FORMAT_BYTES = {
                      'mono8': 1,
                      'mono10': 2,
                      'mono12': 2,
                      'mono14': 2,
                      'mono16': 2,
                     }

IMAGE_EXTENSIONS = (
                    '.bmp',
                    '.tiff',
                    '.tif',
                    '.png',
                    '.jpg',
                    '.jpeg',
                   )


def _write_xiseq_header(f, pixel_format):
    f.write('<ImageSequence version="4.8.2">\n')
    f.write(' <imageMetadata>\n')
    val = PIXEL_FORMAT_VALUES[pixel_format.lower()]
    f.write('  <apiContextList>xiApiImg:format=%d\n' % val)
    f.write('  </apiContextList>\n')
    f.write(' </imageMetadata>\n')


def _write_xiseq_footer(f):
    f.write('</ImageSequence>\n')


def _write_rawm_header(f, endianness, width, height, pixel_format):
    f.write('<?xml version="1.0" encoding="UTF-8" ?>\n')
    f.write('<movie_metadata app_name="xiFastMovie" version="1.4">\n')
    f.write('\t<header>\n')
    f.write('\t\t<width>%d</width>\n' % width)
    f.write('\t\t<height>%d</height>\n' % height)
    f.write('\t\t<pixel_format>%s</pixel_format>\n' % pixel_format.capitalize())
    f.write('\t\t<endianness>%s</endianness>\n' % endianness)
    f.write('\t</header>\n')
    f.write('\t<frames>\n')


def _write_rawm_footer(f):
    f.write('\t</frames>\n')
    f.write('</movie_metadata>\n')


def _get_image_filenames(folder):
    """Returns a nicely ordered list of filenames of the images in the folder."""

    filenames = os.listdir(folder)
    img_filenames = [name for name in filenames if name.endswith(IMAGE_EXTENSIONS)]
    return natsorted(img_filenames)


def folder2xiseq(folder, pixel_format):
    """Builds a .xiseq file from the given folder."""

    folder = folder.rstrip('/').rstrip('\\')
    xi_path = '%s.xiseq' % folder
    base_folder = os.path.split(folder)[1]
    filenames = _get_image_filenames(folder)
    with open(xi_path, 'w') as f:
        _write_xiseq_header(f, pixel_format)
        for timestamp, filename in enumerate(filenames):
            relpath = os.path.join(base_folder, filename)
            f.write(' <file timestamp="%d">%s</file>\n' % (timestamp, relpath))
        _write_xiseq_footer(f)


def folder2raw(folder, pixel_format):
    """Builds .raw and .rawm files from the given folder."""

    folder = folder.rstrip('/').rstrip('\\')
    raw_path = '%s.raw' % folder
    rawm_path = '%s.rawm' % folder
    filenames = _get_image_filenames(folder)

    # Get movie dimensions from first image
    im = PIL.Image.open(os.path.join(folder, filenames[0]))
    width, height = im.size
    # Pixel size consistency check
    data = numpy.asarray(im)
    if data.dtype.itemsize != PIXEL_FORMAT_BYTES[pixel_format]:
        raise TypeError("First image's pixel data do not match the given pixel format.")

    # Write .raw file
    with open(raw_path, 'bw') as f:
        for filename in filenames:
            im = PIL.Image.open(os.path.join(folder, filename))
            data = numpy.asarray(im)
            data.tofile(f)

    # Write .rawm file
    with open(rawm_path, 'w') as f:
        _write_rawm_header(f, sys.byteorder, width, height, pixel_format)
        for i in range(len(filenames)):
            f.write('\t<frame frame="%d" timestamp="%d" />\n' % (i, i))
        _write_rawm_footer(f)
