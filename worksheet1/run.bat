@echo off
set TASK=%1
if "%TASK%"=="" set TASK=all

cd /d "%~dp0src"

if "%TASK%"=="build" goto BUILD
if "%TASK%"=="all" goto BUILD
goto RUNTASKS

:BUILD
echo Building...
nasm -f win32 asm_io.asm -o asm_io.obj
nasm -f win32 task1.asm -o task1.obj
nasm -f win32 task1_2.asm -o task1_2.obj
nasm -f win32 task2_welcome.asm -o task2_welcome.obj
nasm -f win32 task2_array.asm -o task2_array.obj
nasm -f win32 task2_range.asm -o task2_range.obj
gcc -m32 -c driver.c -o driver.obj
gcc -m32 driver.obj task1.obj asm_io.obj -o task1.exe
gcc -m32 driver.obj task1_2.obj asm_io.obj -o task1_2.exe
gcc -m32 driver.obj task2_welcome.obj asm_io.obj -o task2_welcome.exe
gcc -m32 driver.obj task2_array.obj asm_io.obj -o task2_array.exe
gcc -m32 driver.obj task2_range.obj asm_io.obj -o task2_range.exe
echo Build complete.
if "%TASK%"=="build" goto END

:RUNTASKS
if "%TASK%"=="task1" goto T1
if "%TASK%"=="all" goto T1
goto T1_2

:T1
echo --- task1 ---
task1.exe
if "%TASK%"=="task1" goto END

:T1_2
if "%TASK%"=="task1_2" goto T1_2_RUN
if "%TASK%"=="all" goto T1_2_RUN
goto T2W

:T1_2_RUN
echo --- task1_2 ---
task1_2.exe
if "%TASK%"=="task1_2" goto END

:T2W
if "%TASK%"=="task2_welcome" goto T2W_RUN
if "%TASK%"=="all" goto T2W_RUN
goto T2A

:T2W_RUN
echo --- task2_welcome ---
task2_welcome.exe
if "%TASK%"=="task2_welcome" goto END

:T2A
if "%TASK%"=="task2_array" goto T2A_RUN
if "%TASK%"=="all" goto T2A_RUN
goto T2R

:T2A_RUN
echo --- task2_array ---
task2_array.exe
if "%TASK%"=="task2_array" goto END

:T2R
if "%TASK%"=="task2_range" goto T2R_RUN
if "%TASK%"=="all" goto T2R_RUN
goto END

:T2R_RUN
echo --- task2_range ---
task2_range.exe

:END
