#!/usr/bin/env python
import sys
from subprocess import check_call, CalledProcessError


def install_dependencies():
    check_call(['sudo', 'apt-get', 'update', '-qq'])
    check_call([
        'sudo', 'apt-get', 'install', '-y',
        'build-essential',
        'cmake',
        'libssl-dev',
        'libboost-all-dev',
        'libavcodec-dev',
        'libavformat-dev',
        'libavutil-dev',
        'libavfilter-dev',
        'libavdevice-dev',
        'libswresample-dev',
        'libswscale-dev',
        'fluid',
        'libfltk1.3-dev',
        'libgl-dev',
        'libjsoncpp-dev',
        'libopencv-dev',
        'lcov',
        'python3-pip',
    ])
    check_call([
        'sudo', 'pip3', 'install',
        'setuptools',
        'pre-commit',
    ])


if __name__ == '__main__':
    try:
        install_dependencies()
    except CalledProcessError:
        sys.exit(1)
    sys.exit(0)