[![Build Status](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml)

Toucan
======

Toucan is a project for experimenting with rendering timelines. The project
currently consists of a command line program that can render timelines into
image sequences.

Features:
* Composite multiple video tracks with effects and transitions
* [OpenColorIO](https://github.com/AcademySoftwareFoundation/OpenColorIO) transforms applied to individual clips and tracks
* [OpenFX](https://github.com/AcademySoftwareFoundation/openfx) based plugin system for image generators, effects, and transitions
* [OpenImageIO](https://github.com/AcademySoftwareFoundation/OpenImageIO) image I/O and processing

Image Effects
=============
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
