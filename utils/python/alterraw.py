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


"""Alters a raw/rawm movie and save it into new files.

Possible operations are cutting movie to a smaller sequence and changing
frames step.
"""


import os
import sys
import argparse
import corrtrack.conversions as conversions


if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Alters a raw/rawm movie and saves it into new files.')
    parser.add_argument('input', type=str,
                        help='input .rawm file')
    parser.add_argument('output', type=str,
                        nargs='?',
                        help='input .rawm file')
    parser.add_argument('--begin', type=int,
                        default=1,
                        help='start frame')
    parser.add_argument('--end', type=int,
                        help='end frame')
    parser.add_argument('--step', type=int,
                        default=1,
                        help='frame step')
    parser.add_argument('--x', type=int,
                        default=1,
                        help='crop window x')
    parser.add_argument('--y', type=int,
                        default=1,
                        help='crop window y')
    parser.add_argument('--width', type=int,
                        help='crop window width')
    parser.add_argument('--height', type=int,
                        help='crop window height')
    args = parser.parse_args()

    # Set default values and validate arguments
    #
    # input
    if os.path.splitext(args.input)[1] != '.rawm':
        sys.stderr.write('Input file extension must be ".rawm".\n')
        sys.exit(2)
    input = os.path.splitext(args.input)[0]
    #
    # output
    if args.output is not None:
        if os.path.splitext(args.output)[1] != '.rawm':
            sys.stderr.write('Output file extension must be ".rawm".\n')
            sys.exit(2)
        output = os.path.splitext(args.output)[0]
    else:
        output = os.path.splitext(input)[0] + '_out'
    #
    metadata = conversions.Metadata(input)
    #
    # begin
    begin = args.begin
    # Validate the begin, end and step parameters
    if begin < 1 or begin > metadata.n_frames:
        sys.stderr.write('The --begin parameter must be in (1, n_frames).\n')
        sys.exit(2)
    #
    # end
    end = args.end
    if end is None:
        end = metadata.n_frames
    if end < 1 or end > metadata.n_frames:
        sys.stderr.write('The --end parameter must be in (1, n_frames).\n')
        sys.exit(2)
    if end < begin:
        sys.stderr.write('The --end parameter must be greater or equal than --begin.\n')
        sys.exit(2)
    #
    # step
    step = args.step
    if step < 1:
        sys.stderr.write('The --step parameter must be greater than zero.\n')
        sys.exit(2)
    #
    # x
    x = args.x
    if x < 1 or x > metadata.width:
        sys.stderr.write('The --x parameter must be in (1, image_width).\n')
        sys.exit(2)
    #
    # y
    y = args.y
    if y < 1 or y > metadata.height:
        sys.stderr.write('The --y parameter must be in (1, image_height).\n')
        sys.exit(2)
    #
    # width
    width = args.width
    if width is None:
        width = metadata.width - (x - 1)
    if width < 1:
        sys.stderr.write('The --width parameter must be greater or equal to 1.\n')
        sys.exit(2)
    if x + width - 1 > metadata.width:
        sys.stderr.write('Cropping window right side is outside image boundaries.\n')
        sys.exit(2)
    #
    # height
    height = args.height
    if height is None:
        height = metadata.height - (y - 1)
    if height < 1:
        sys.stderr.write('The --height parameter must be greater or equal to 1.\n')
        sys.exit(2)
    if y + height - 1 > metadata.height:
        sys.stderr.write('Cropping window bottom side is outside image boundaries.\n')
        sys.exit(2)


    # Check that output files do not exist
    if os.path.exists(output + '.rawm'):
        sys.stderr.write('Output .rawm file already exists.\n')
        sys.exit(1)
    if os.path.exists(output + '.raw'):
        sys.stderr.write('Output .raw file already exists.\n')
        sys.exit(1)

    conversions.alter_movie(input, output, begin, end, step,
                            x, y, width, height)
