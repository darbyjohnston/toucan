[![Build Status](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml)

Toucan
======

Toucan is a project for experimenting with timeline rendering. The project
currently consists of a command line renderer that can process timelines
with image sequences, effects, and transitions.

Dependencies:
* [OpenTimelineIO](https://github.com/AcademySoftwareFoundation/OpenTimelineIO) - Timelines
* [OpenImageIO](https://github.com/AcademySoftwareFoundation/OpenImageIO) - Image I/O and processing
* [OpenFX](https://github.com/AcademySoftwareFoundation/openfx) - Plugins

Example Renders
===============
Compositing multiple tracks:

![CompositeTracks](images/CompositeTracks.png)

![Composite Tracks Graph](images/CompositeTracksGraph.svg)

Filter effects; color map, invert, power, and saturate:

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
