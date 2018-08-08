# smart-vcutter
A multiplatform tool to generate small, silent and short clips from videos.

![Ubuntu screenshot](https://raw.githubusercontent.com/rodjjo/smart-vcutter/master/docs/images/ubuntu-screen-shot.png)

Attention: The sound support is not necessary to this project. The focus is to produce video animations instead of GIF images. I hope gif format get out of the internet some day.

It was a prototype (XGH) and needs lots of code improvements, i'm opening the code to get some help with that.

## Features

* Extract a smaller area from video frame.
* Rotation, Scale, Translation support.
* Reverse video playback.
* h-264, h-265, mjpeg, webm codecs support
* First/Last frame comparation tool.
* Interpolation support.
* User friendly interface.
* Multiplatform.

**Example of a generated video clip**:

[![example](http://img.youtube.com/vi/7MCendkxo1I/0.jpg)](http://www.youtube.com/watch?v=7MCendkxo1I)

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
    libjsoncpp-dev \
    libopencv-dev
```

Generate and build the project

```bash
cmake .
cmake --build .
```

# If you are a developer and want to colaborate

Install python 3.5 or above  
Install pre-commit  

* Turn the functions smaller.
* Remove repeated code.
* Refactory complex code.
* Rename functions and variables to make the project easy to understand.
* Create documentations.
* Add tests when it's possible.

# My TODO list

* Create a user manual.
* Generate a .deb package.
* Add option to run after setup.
* Add option to associate files on setup.
* Disable menus when all submenus are disabled (except root ones).
* Remove repeated code and create functions.
* Iconize all menu items with xpm.
* Create a preview window to show how the generated file will be.
* Remove some scale and rotation menus and let the user choose the value.
* Detect up-scale offer to resize the output optionally.
* Create an about dialog to show colaborators.
* Increase the plaback speed button width.
* Add support to aligment operations (makes the selected box fit horizontally, fit vertically and other operations)
* When drag a file to the main window, try to open it. (if fltk supports)
