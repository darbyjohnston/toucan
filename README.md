[![Build Status](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml)

toucan
======

Toucan is a project for experimenting with timeline rendering. The project
currently consists of a C++ software renderer that can render basic timelines
with image sequences, effects, and transitions.

The OpenTimelineIO library is used for working with timelines, and the
OpenImageIO library is used for image I/O and image processing.

Effects
=======

Patterns: Fill, Checkers, Noise

Filters: Invert, Saturation

Transforms: Flip/flop, Resize, Rotate

Miscellaneous: Linear time warp, Lines, Boxes, Text

Example Renders
===============
Compositing multiple tracks:

![CompositeTracks](images/CompositeTracks.png)

![Composite Tracks Graph](images/CompositeTracksGraph.svg)

Filter effects; invert and saturate:

![Filters](images/Filters.png)

Gap:

![Gap](images/Gap.png)

Linear time warps:

![LinearTimeWarp](images/LinearTimeWarp.png)

Patterns; fill, checkers, and noise:

![Patterns](images/Patterns.png)

Rendering; line, box, and text:

![Render](images/Render.png)

Transition:

![Transition](images/Transition.png)

![Transition Graph](images/TransitionGraph.svg)

Transition with an invert effect on the clips:

![Transition 2](images/Transition2.png)

![Transition 2 Graph](images/Transition2Graph.svg)

Transforms; resize, rotate, flip, and flop:

![Transforms](images/Transforms.png)

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
