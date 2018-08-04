# smart-vcutter
A multiplatform tool to generate small silent and short video clips from videos.

It was a prototype and needs lots of code improvements, i'm opening the code to get some help with that.

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

# If you are a developer and want to colaborate:

* Turn the functions smaller.
* Remove repeated code.
* Refactory complex code.
* Rename functions and variables to make the project easy to understand.
* Create documentations.
* Add tests when it's possible.
