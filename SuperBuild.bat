set BUILD_TYPE=%1
IF "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

cmake -S toucan\cmake\SuperBuild -B superbuild-%BUILD_TYPE% -DCMAKE_INSTALL_PREFIX=%CD%\install-%BUILD_TYPE% -DCMAKE_PREFIX_PATH=%CD%\install-%BUILD_TYPE% -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
cmake --build superbuild-%BUILD_TYPE% -j 4 --config %BUILD_TYPE%

cmake -S toucan -B build-%BUILD_TYPE% -DCMAKE_INSTALL_PREFIX=%CD%\install-%BUILD_TYPE% -DCMAKE_PREFIX_PATH=%CD%\install-%BUILD_TYPE% -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
cmake --build build-%BUILD_TYPE% -j 4 --config %BUILD_TYPE%
cmake --build build-%BUILD_TYPE% --target INSTALL

