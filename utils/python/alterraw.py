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

Possible operations are cropping to a smaller sequence, framerate change.  TODO
continue.
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
                        default=max,
                        help='end frame')
    parser.add_argument('--step', type=int,
                        default=1,
                        help='frame step')

    args = parser.parse_args()

    # Validate arguments
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
        exit(2)
    #
    # end
    end = args.end
    # Determine end frame if it is set to the max value
    if type(end) is not int:
        end = metadata.n_frames
    if end < 1 or end > metadata.n_frames:
        sys.stderr.write('The --end parameter must be in (1, n_frames).\n')
        exit(2)
    if end < begin:
        sys.stderr.write('The --end parameter must be greater or equal than --begin.\n')
        exit(2)
    #
    # step
    step = args.step
    if step < 1:
        sys.stderr.write('The --step parameter must be greater than zero.\n')
        exit(2)

    # Check that output files do not exist
    if os.path.exists(output + '.rawm'):
        sys.stderr.write('Output .rawm file already exists.\n')
        sys.exit(1)
    if os.path.exists(output + '.raw'):
        sys.stderr.write('Output .raw file already exists.\n')
        sys.exit(1)

    conversions.alter_movie(input, output, begin, end, step)
