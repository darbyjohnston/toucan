toucan
======

Building
========

Building on Windows
-------------------
Clone the repository:
```
git clone https://github.com/darbyjohnston/toucan.git
```
Build the dependencies:
```
cmake -S toucan\cmake\SuperBuild -B SuperBuild-Release -DCMAKE_INSTALL_PREFIX=%CD%\install -DCMAKE_PREFIX_PATH=%CD%\install -DCMAKE_BUILD_TYPE=Release
```
```
cmake --build SuperBuild-Release -j 4 --config Release
```
Build the project:
```
cmake -S toucan -B Build-Release -DCMAKE_INSTALL_PREFIX=%CD%\install -DCMAKE_PREFIX_PATH=%CD%\install -DCMAKE_BUILD_TYPE=Release
```
```
cmake --build Build-Release -j 4 --config Release
```
