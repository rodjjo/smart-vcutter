# smart-vcutter
A multiplatform tool to generate small silent and short video clips from videos.


## Features

* Extract a smaller area from video frame.
* Rotation, Scale, Translation support.
* Reverse video playback.
* h-264, h-265, mjpeg, webm codecs support
* First/Last frame comparation tool.
* Interpolation support.
* User friendly interface.
* Multiplatform.


## Build

Install the building tools

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake
```

Install the dependencies:

```bash
sudo apt-get update
sudo apt-get update -qq && sudo apt-get -y install \
    libboost-all-dev \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libavfilter-dev \
    libavdevice-dev \
    libswresample-dev \
    libswscale-dev \
    fluid \
    libfltk1.3-dev \
    libgl-dev \
    libjsoncpp-dev
```

Generate and build the project

```bash
cmake .
cmake --build .
```
