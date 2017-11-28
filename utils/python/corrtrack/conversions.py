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
import lxml.etree
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


class Metadata(object):
    """Class that parses and store data from the XML tree in a .rawm file

    This currently only parses movie dimensions, but the class may be extended
    in the future.
    """

    def __init__(self, path):
        root = _get_rawm_tree(path + '.rawm')
        header = _get_elem(root, 'header')

        self.width = int(_get_elem(header, 'width').text)
        self.height = int(_get_elem(header, 'height').text)
        try:
            self.pixel_fmt = _get_elem(header, 'pixel_format').text
        except KeyError:
            self.pixel_fmt = 'Mono8'
        if self.pixel_fmt not in ['Mono8', 'Mono10', 'Mono12',
                                  'Mono14', 'Mono16']:
            raise ValueError('Unknown "pixel_format" parameter value.')


        frames_tree = _get_elem(root, 'frames')
        frames = frames_tree.findall('frame')
        self.n_frames = len(frames)

        # Validate .raw access and file size
        try:
            raw_size = os.path.getsize(path + '.raw.')
        except OSError:
            sys.stderr.write('Unable to read .raw file.\n')
            sys.exit(1)
        if raw_size != self.n_frames * self.frame_size():
            sys.stderr.write('.raw file size does not match dimensions from .rawm file.\n')
            sys.exit(1)


    def bytes_per_pixel(self):
        return 1 if self.pixel_fmt == 'Mono8' else 2


    def frame_size(self):
        return self.width * self.height * self.bytes_per_pixel()


def _get_rawm_tree(path):
    """Open an XML .rawm file and return the root tree."""

    root = lxml.etree.parse(path).getroot()
    if root.tag != 'movie_metadata':
        raise KeyError('The XML root is not a "movie_metadata" element.')
    return root


def _create_xml_with_header(endianness, width, height, pixel_format):
    """Creates a movie_metadata XML structure with filled header and empty 'frames'"""

    root = lxml.etree.Element('movie_metadata',
                              app_name='xiFastMovie',
                              version='1.4')
    header = lxml.etree.SubElement(root, 'header')

    lxml.etree.SubElement(header, 'width').text = '%d' % width
    lxml.etree.SubElement(header, 'height').text = '%d' % height
    lxml.etree.SubElement(header, 'pixel_format').text = pixel_format.capitalize()
    lxml.etree.SubElement(header, 'endianness').text = endianness

    lxml.etree.SubElement(root, 'frames')

    return root


def _xml_add_frame(root, frame, timestamp):
    """Adds a frame element to the frames structure in the XML metadata"""

    frames = root.find('frames')
    lxml.etree.SubElement(frames, 'frame',
                          frame='%d' % frame,
                          timestamp='%d' % timestamp)


def _create_xiseq_with_header(pixel_format):
    """Creates a Xiseq XML structure with filled header"""

    root = lxml.etree.Element('ImageSequence', version='4.8.2')
    header = lxml.etree.SubElement(root, 'imageMetadata')

    val = PIXEL_FORMAT_VALUES[pixel_format.lower()]
    lxml.etree.SubElement(header, 'apiContextList').text = 'xiApiImg:format=%d' % val

    return root


def _xiseq_add_frame(root, timestamp, relpath):
    """Adds a frame ("file" tag) to the XML metadata"""

    file = lxml.etree.SubElement(root, 'file', timestamp='%d' % timestamp)
    file.text = relpath


def _get_image_filenames(folder):
    """Returns a nicely ordered list of filenames of the images in the folder."""

    filenames = os.listdir(folder)
    img_filenames = [name for name in filenames if name.endswith(IMAGE_EXTENSIONS)]
    return natsorted(img_filenames)


def _get_elem(tree, key):
    """Returns the an XML element or raises an appropriate exception"""

    elem = tree.find(key)
    if elem is None:
        raise KeyError('No "%s" element found in the XML data.' % key)
    return elem


def _get_attr(tree, attribute):
    """Returns the an XML attribute or raises an appropriate exception"""

    value = tree.get(attribute)
    if value is None:
        raise KeyError('No "%s" attribute found in the "%s" XML element.' % (attribute,
                                                                             tree.tag))
    return value


def folder2xiseq(folder, pixel_format):
    """Builds a .xiseq file from the given folder."""

    folder = os.path.abspath(folder)
    xi_path = '%s.xiseq' % folder
    base_folder = os.path.split(folder)[1]
    filenames = _get_image_filenames(folder)

    root = _create_xiseq_with_header(pixel_format)
    for timestamp, filename in enumerate(filenames):
        relpath = os.path.join(base_folder, filename)
        _xiseq_add_frame(root, timestamp, relpath)

    tree = lxml.etree.ElementTree(root)
    tree.write(xi_path, encoding='UTF-8', xml_declaration=True,
               pretty_print=True)


def folder2raw(folder, pixel_format):
    """Builds .raw and .rawm files from the given folder."""

    folder = os.path.abspath(folder)
    raw_path = '%s.raw' % folder
    rawm_path = '%s.rawm' % folder
    filenames = _get_image_filenames(folder)

    # Get movie dimensions from first image
    im = PIL.Image.open(os.path.join(folder, filenames[0]))
    width, height = im.size
    data = numpy.asarray(im)
    # Only accept grayscale images
    if len(data.shape) == 3:
        if data.shape[2] != 1:
            raise TypeError('First image is not a grayscale image.')
    # Pixel size consistency check
    if data.dtype.itemsize != PIXEL_FORMAT_BYTES[pixel_format]:
        raise TypeError("First image's pixel data do not match the given pixel format.")

    # Write .raw file
    with open(raw_path, 'bw') as f:
        for filename in filenames:
            im = PIL.Image.open(os.path.join(folder, filename))
            data = numpy.asarray(im)
            data.tofile(f)

    # Write .rawm file
    root = _create_xml_with_header(sys.byteorder,
                                   width, height, pixel_format)

    for i in range(len(filenames)):
        _xml_add_frame(root, i, i)

    tree = lxml.etree.ElementTree(root)
    print(rawm_path)
    tree.write(rawm_path, encoding='UTF-8', xml_declaration=True,
               pretty_print=True)


def alter_movie(input, output, begin, end, step):
    """Modifies a movie and saves the result into new files"""

    metadata = Metadata(input)

    # Write output .raw file
    with open(input + '.raw', 'rb') as file_in:
        with open(output + '.raw', 'wb') as file_out:
            # Make sure to keep the following logic up-to-date with the one for
            # the metadata.
            for i in range(begin - 1, end, step):
                cursor = i * metadata.frame_size()
                file_in.seek(cursor)
                file_out.write(file_in.read(metadata.frame_size()))

    root = _get_rawm_tree(input + '.rawm')
    frames_tree = _get_elem(root, 'frames')
    childs = frames_tree[:]
    i = len(childs) - 1
    for child in reversed(childs):
        if child.tag == 'frame':
            # Make sure to keep the following logic up-to-date with the one for
            # the movie data.
            if i < begin - 1 or i > end -1 or (i - (begin - 1)) % step != 0:
                frames_tree.remove(child)
            i -= 1

    # Update framerate if known
    header = _get_elem(root, 'header')
    try:
        elem = _get_elem(header, 'framerate')
    except KeyError:
        pass
    else:
        framerate = float(elem.text)
        elem.text = str(framerate / step)

    # Write new XML file
    tree = lxml.etree.ElementTree(root)
    tree.write(output + '.rawm', encoding='UTF-8', xml_declaration=True,
               pretty_print=True)
