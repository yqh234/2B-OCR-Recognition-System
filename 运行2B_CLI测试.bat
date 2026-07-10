@echo off
setlocal
set "PATH=C:\msys64\mingw64\bin;%PATH%"
set "TESSDATA_PREFIX=C:\msys64\mingw64\share\tessdata"
cd /d "%~dp0"
cpp\build\ocr_cli.exe test_samples ocr_output_full_test ocr_results_full_test.csv
pause
