@echo off

set cmake_exe=D:/common/tools/cmake/cmake-3.15.3-win64-x64/bin/cmake.exe
set src_dir=.
set build_dir=build

rmdir /s /q %build_dir%
mkdir %build_dir%

REM Generate Build system
%cmake_exe% ^
    -G "Visual Studio 16 2019" -A x64 ^
    -B%build_dir% -S%src_dir%

REM Open project in IDE
%cmake_exe% --open %build_dir%
