@echo off
set PATH=C:\msys64\mingw64\bin;%PATH%
set TESSDATA_PREFIX=C:\msys64\mingw64\share\tessdata
cd /d "%~dp0"
set WORK=C:\Users\86136\Desktop\program\vision_practice_windows\combined_demo_work
if exist "%WORK%" rmdir /s /q "%WORK%"
mkdir "%WORK%\input"
mkdir "%WORK%\output"
mkdir "%WORK%\result"
xcopy /y /q "07_综合检测Demo\01_综合测试原图\*.png" "%WORK%\input\" >nul
06_源代码\cpp\build\comprehensive_demo.exe "%WORK%\input" "%WORK%\output" "%WORK%\result\comprehensive_results.csv"
if not exist "07_综合检测Demo\02_综合标注输出图" mkdir "07_综合检测Demo\02_综合标注输出图"
if not exist "07_综合检测Demo\03_综合检测结果表格" mkdir "07_综合检测Demo\03_综合检测结果表格"
xcopy /y /q "%WORK%\output\*.jpg" "07_综合检测Demo\02_综合标注输出图\" >nul
copy /y "%WORK%\result\comprehensive_results.csv" "07_综合检测Demo\03_综合检测结果表格\综合检测结果.csv" >nul
C:\Users\86136\Anaconda3\python.exe 06_源代码\export_csv_to_xlsx.py 07_综合检测Demo\03_综合检测结果表格\综合检测结果.csv 07_综合检测Demo\03_综合检测结果表格\综合检测结果.xlsx
pause
