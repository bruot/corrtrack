#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This file is part of the particle tracking software CorrTrack.
#
# Copyright 2018 Nicolas Bruot and CNRS
# Copyright 2016-2018 Nicolas Bruot
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
Deployment parameters for CorrTrack

This is used by deploy.py.
"""


QT_VERSION = '5.11.0'

VS_INSTALL_DIR = 'C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC'

QT_BIN_DIR = {
              'x86': 'C:/Qt/%s/msvc2015/bin' % QT_VERSION,
              'x64': 'C:/Qt/%s/msvc2015_64/bin' % QT_VERSION,
             }

FILES = {
         'common': (
                    ('./deployment_README', 'README'),
                    ('../COPYING', ''),
                    ('../utils/python/corrtrack/__init__.py', 'utils/python/corrtrack/__init__.py'),
                    ('../utils/python/corrtrack/conversions.py', 'utils/python/corrtrack/conversions.py'),
                    ('../utils/python/alterraw.py', 'utils/python/alterraw.py'),
                    ('../utils/python/folder2raw.py', 'utils/python/folder2raw.py'),
                    ('../utils/python/folder2xiseq.py', 'utils/python/folder2xiseq.py'),
                    ('../../additions/sample_files/2.10/movie.rawm', 'examples/movie.rawm'),
                    ('../../additions/sample_files/2.10/movie.raw', 'examples/movie.raw'),
                    ('../../additions/sample_files/2.10/kernel.dat', 'examples/kernel.dat'),
                   ),
         'x86': (
                 ('C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/redist/x86/Microsoft.VC140.CRT/msvcp140.dll', ''),
                 ('C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/redist/x86/Microsoft.VC140.CRT/vcruntime140.dll', ''),
                 ('C:/lib/msvc2015_32/lib/tiff/tiff.dll', ''),
                 ('C:/lib/msvc2015_32/lib/zlib/zlib.dll', ''),
                ),
         'x64': (
                 ('C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/redist/x64/Microsoft.VC140.CRT/msvcp140.dll', ''),
                 ('C:/Program Files (x86)/Microsoft Visual Studio 14.0/VC/redist/x64/Microsoft.VC140.CRT/vcruntime140.dll', ''),
                 ('C:/lib/msvc2015_64/lib/tiff/tiff.dll', ''),
                 ('C:/lib/msvc2015_64/lib/zlib/zlib.dll', ''),
                ),
        }

OUTPUT_DIR = '../../deployment'
