#!/usr/bin/env python3
import os
import sys
from subprocess import check_call, CalledProcessError

BASE_PATH = './build/bin/smartvcutter'
DEBIAN_PATH = f'{BASE_PATH}/DEBIAN'
BIN_PATH = '/usr/local/bin/'
DEB_BIN_PATH = f'{BASE_PATH}{BIN_PATH}'


def create_dirs(path):
    try:
        os.makedirs(path)
    except FileExistsError:
        pass


def write_control_file():
    dependencies = ','.join([
        'apt',
        'libboost-chrono-dev',
        'libboost-filesystem-dev',
        'libboost-system-dev',
        'libboost-timer-dev',
        'libboost-thread-dev',
        'libboost-test-dev',
        'libavcodec-dev',
        'libavformat-dev',
        'libavutil-dev',
        'libavfilter-dev',
        'libavdevice-dev',
        'libswresample-dev',
        'libswscale-dev',
        'libfltk1.3-dev',
        'libgl1-mesa-dev',
        'libjsoncpp-dev',
        'libopencv-dev',
    ])

    contents = '\n'.join([
        'Package: smart-vcutter',
        'Version: 1.0.2',
        'Section: video',
        f'Depends: {dependencies}',
        'Priority: optional',
        'Architecture: amd64',
        'Maintainer: Rodrigo Araujo<rodjjo@gmail.com>',
        'Homepage: https://github.com/rodjjo/smart-vcutter/',
        'Provides: smart-vcutter',
        'Conflicts: smart-vcutter',
        'Replaces: smart-vcutter',
        'Description: Generate small video loops from videos\n',
    ])

    with open(f'{BASE_PATH}/DEBIAN/control', 'w') as ctrlfile:
        ctrlfile.write(contents)


def write_copyright_file():
    contents = [
        'Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/',
        'Upstream-Name: rodrigo',
        'Upstream-Contact: rodrigo araujo <rodjjo@gmail.com>',
        'Source: https://github.com/rodjjo/smart-vcutter/',
        'Files: *',
        'Copyright: 2018-2018 rodrigo araujo',
        'License: GPL-3',
    ]
    with open(f'{BASE_PATH}/DEBIAN/copyright', 'w') as licfile:
        licfile.write('\n'.join(contents))
        with open('LICENSE', 'r') as infile:
            for line in infile:
                licfile.write(line)

def write_postrm():
    contents = [
        '#!/bin/bash',
        f'rm -f {BIN_PATH}smart-vcutter',
    ]

    with open(f'{BASE_PATH}/DEBIAN/postrm', 'w') as ofile:
        ofile.write('\n'.join(contents))
    check_call(['chmod', '+x', f'{DEBIAN_PATH}/postrm'])


def generate_dist():
    create_dirs(DEBIAN_PATH)
    create_dirs(DEB_BIN_PATH)
    write_control_file()
    write_copyright_file()
    write_postrm()
    
    check_call(['cp', './build/bin/smart-vcutter', f'{BASE_PATH}{BIN_PATH}'])
    check_call(['dpkg-deb', '--build', BASE_PATH])


if __name__ == '__main__':
    try:
        generate_dist()
    except CalledProcessError:
        sys.exit(1)
    sys.exit(0)