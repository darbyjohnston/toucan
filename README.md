[![Build Status](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/darbyjohnston/toucan/actions/workflows/ci-workflow.yml)

toucan
======

Toucan is a project for experimenting with timeline rendering. The project
currently consists of a C++ software renderer that can render basic timelines
with image sequences, effects, and transitions.

Supported effects:

|Patterns|Filters   |Transforms|Misc            |
|--------|----------|----------|----------------|
|Fill    |Invert    |Flip      |Linear Time Warp|
|Checkers|Saturation|Flop      |Text            |
|Noise   |          |Resize    |                |
|        |          |Rotate    |                |

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
