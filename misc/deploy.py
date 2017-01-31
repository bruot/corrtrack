#!/usr/bin/env python
# -*- coding: utf-8 -*-

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
Deploys an executable for Windows.

This script uses program-dependent parameters in deploy_params.py.
"""


import os
import sys
import errno
if sys.version_info < (3, 0):
    from exceptions import OSError
import shutil

import deploy_params


def copy_dependencies(dependencies, dest_dir):

    for src_path, dest_rel_path in dependencies:
        if dest_rel_path:
            dest_path = os.path.join(dest_dir, dest_rel_path)
        else:
            dest_path = os.path.join(dest_dir, os.path.split(src_path)[1])
        dir = os.path.dirname(dest_path)
        try:
            os.makedirs(dir)
        except OSError as exception:
            if exception.errno != errno.EEXIST:
                raise
        print(os.path.normpath(dest_path))
        shutil.copyfile(src_path, dest_path)


if __name__ == '__main__':

    # Check arguments
    if len(sys.argv) != 2:
        sys.stderr.write('syntax: ./deploy.py EXEC_PATH\n')
        exit(1)
    exec_path = sys.argv[1]
    arch = 'x64' if '64bit' in exec_path else 'x86'

    # Clean output directory
    if os.path.exists(deploy_params.OUTPUT_DIR):
        shutil.rmtree(deploy_params.OUTPUT_DIR)
    os.makedirs(deploy_params.OUTPUT_DIR)

    # Copy executable
    exec_path_out = os.path.join(deploy_params.OUTPUT_DIR,
                                 os.path.basename(exec_path))
    shutil.copyfile(exec_path, exec_path_out)
    print(os.path.normpath(exec_path_out))

    copy_dependencies(deploy_params.FILES['common'], deploy_params.OUTPUT_DIR)
    copy_dependencies(deploy_params.FILES[arch], deploy_params.OUTPUT_DIR)
