@echo off
setlocal enabledelayedexpansion
REM Windows batch file equivalent to: make run-direct

echo Starting QEMU with kernel directly...

REM Check if kernel exists
if not exist "iso\boot\kernel.elf" (
    echo Error: Kernel not found at iso\boot\kernel.elf
    echo Attempting to build kernel...
    
    REM Check if make is available and build kernel
    where make >nul 2>&1
    if !ERRORLEVEL! EQU 0 (
        echo Building kernel with make...
        make iso\boot\kernel.elf
        if !ERRORLEVEL! NEQ 0 (
            echo Error: Failed to build kernel
            exit /b 1
        )
    ) else (
        echo Error: Make not found. Please build the kernel first using: make iso/boot/kernel.elf
        exit /b 1
    )
)

REM Find QEMU executable - try PATH first, then common installation locations
set "QEMU_EXE="
where qemu-system-i386.exe >nul 2>&1
if !ERRORLEVEL! EQU 0 (
    set "QEMU_EXE=qemu-system-i386.exe"
    goto :run_qemu
)

if exist "C:\Program Files\qemu\qemu-system-i386.exe" (
    set "QEMU_EXE=C:\Program Files\qemu\qemu-system-i386.exe"
    goto :run_qemu
)

if exist "C:\Program Files (x86)\qemu\qemu-system-i386.exe" (
    set "QEMU_EXE=C:\Program Files (x86)\qemu\qemu-system-i386.exe"
    goto :run_qemu
)

echo Error: qemu-system-i386.exe not found
echo Please ensure QEMU is installed and in your PATH, or at C:\Program Files\qemu
exit /b 1

:run_qemu
REM Run QEMU with kernel directly
"%QEMU_EXE%" -kernel iso\boot\kernel.elf -m 32
