[![Build Status](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml)

Toucan
======

Toucan is an experimental project for rendering timelines. The project
currently consists of:
* C++ library for rendering timelines
* OpenFX image effect plugins
* Command line renderer
* Interactive viewer
* Example .otio files

OpenFX Image Effect Plugins
===========================
* Generators: Checkers, Fill, Gradient, Noise
* Drawing: Box, Line, Text
* Filters: Blur, Color Map, Invert, Power, Saturate, Unsharp Mask
* Transforms: Crop, Flip, Flop, Resize, Rotate
* Transitions: Dissolve, Horizontal Wipe, Vertical Wipe
* Color spaces: Color Convert, Premultiply Alpha, Un-Premultiply Alpha

Example Renders
===============
Compositing multiple tracks:

![CompositeTracks](images/CompositeTracks.png)

![Composite Tracks Graph](images/CompositeTracksGraph.svg)

Filter effects; color map, invert, power, saturate, blur, and unsharp mask:

![Filters](images/Filter.png)

Gap:

![Gap](images/Gap.png)

Linear time warps:

![LinearTimeWarp](images/LinearTimeWarp.png)

Generators; fill, gradient, checkers, and noise:

![Generators](images/Generator.png)

Drawing; line, box, and text:

![Render](images/Draw.png)

Dissolve transition:

![Transition](images/Transition.png)

![Transition Graph](images/TransitionGraph.svg)

Dissolve transition with an invert effect on the clips:

![Transition 2](images/Transition2.png)

![Transition 2 Graph](images/Transition2Graph.svg)

Wipe transitions:

![Transition Wipe](images/TransitionWipe.png)

Transforms; resize, rotate, flip, and flop:

![Transforms](images/Transform.png)

Color space conversion:

![Color Space](images/ColorSpace.png)

Multiple effects on clips, tracks, and stacks:

![Track Effects](images/MultipleEffects.png)

![Multiple Effects Graph](images/MultipleEffectsGraph.svg)

FFmpeg Encoding
===============
Toucan currently only writes image sequences, but these can be converted
into movie files with FFmpeg. The image sequences can be written to disk
and then converted with FFmpeg, or they can be piped directly to FFmpeg
without the overhead of disk I/O.

Example command line for piping images to FFmpeg:
```
toucan-render Transition.otio - -raw rgba | ffmpeg -y -f rawvideo -pix_fmt rgba -s 1280x720 -r 24 -i pipe: output.mov
```
* The "-" argument uses standard out instead of an output file.
* The "-raw rgba" option sets the pixel format of the frames. This should
match the "-pix_fmt" option given to FFmpeg. One exception is that toucan
options do not specify the endian, the endian of the current machine is used.
So for example the toucan option "-raw rgbaf16" might match the FFmpeg option
"-pix_fmt rgbaf16le" for the current machine. Check the toucan-render command
line help for the list of available formats.
* The "-y" option overwrites the output file if it already exists.
* The "-f rawvideo" option sets the input to raw video frames.
* The "-pix_fmt rgba" option sets the input pixel format as described above.
* The "-s 1280x720" option sets the size of the input frames. The toucan-render
can be used to find the image size with the "-print_size" option.
* The "-r 24" option sets the frame rate. The toucan-render can be used to
find the frame rate with the "-print_rate" option.
* The "-i pipe:" option uses standard input instead of a file.
* The "output.mov" is the output movie file.

Building
========

Building on Linux
-----------------
Clone the repository:
```
git clone https://github.com/darbyjohnston/toucan.git
```
Run the super build:
```
sh toucan/SuperBuild.sh Release
```
Run the viewer application:
```
export LD_LIBRARY_PATH=$PWD/install-Release/lib:$LD_LIBRARY_PATH
```
```
install-Release/bin/toucan-view toucan/data/Transition.otio
```

Building on macOS
-----------------
Clone the repository:
```
git clone https://github.com/darbyjohnston/toucan.git
```
Run the super build:
```
sh toucan/SuperBuild.sh Release
```
Run the viewer application:
```
install-Release/bin/toucan-view toucan/data/Transition.otio
```

Building on Windows
-------------------
Clone the repository:
```
git clone https://github.com/darbyjohnston/toucan.git
```
Run the super build:
```
toucan\SuperBuild.bat Release
```
Run the viewer application:
```
set PATH=%CD%\install-Release\bin;%PATH%
```
```
install-Release\bin\toucan-view.exe toucan\data\Transition.otio
```

