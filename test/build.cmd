@echo off
REM Configure and open the cmdp project in Visual Studio.

setlocal
set src_dir=%~dp0..
set build_dir=%src_dir%\build
set generator=Visual Studio 17 2022

if "%1"=="--clean" if exist "%build_dir%" rmdir /s /q "%build_dir%"

REM Generate build system
cmake -G "%generator%" -A x64 -S "%src_dir%" -B "%build_dir%"
if errorlevel 1 exit /b 1

REM Build and run tests
cmake --build "%build_dir%" --config Debug
if errorlevel 1 exit /b 1

cd /d "%build_dir%"
ctest -C Debug --output-on-failure
endlocal
