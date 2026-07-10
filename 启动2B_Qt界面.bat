@echo off
setlocal
set "PATH=C:\msys64\mingw64\bin;%PATH%"
set "TESSDATA_PREFIX=C:\msys64\mingw64\share\tessdata"
cd /d "%~dp0"
start "" "%~dp0cpp\build\ocr_qt.exe"
