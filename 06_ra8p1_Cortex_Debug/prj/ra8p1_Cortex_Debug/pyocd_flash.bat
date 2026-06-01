@echo off
setlocal enabledelayedexpansion

set PYOCD_DIR=D:\Renesas\e2_studio\pyocd
set TARGET=R7KA8P1KF
set FREQ=1000000

set SCRIPT_DIR=%~dp0
set ELF_FILE=%SCRIPT_DIR%Debug\ra8p1_rtt.elf
set SREC_FILE=%SCRIPT_DIR%Debug\ra8p1_rtt.srec

if not exist "%ELF_FILE%" (
    echo [ERROR] ELF file not found: %ELF_FILE%
    echo Please build the project first in e2studio.
    pause
    exit /b 1
)

echo ============================================
echo  PyOCD Flash Tool for RA8P1 (DAPLink)
echo ============================================
echo  Target:  %TARGET%
echo  File:    %ELF_FILE%
echo  Adapter: DAPLink (CMSIS-DAP)
echo ============================================

cd /D "%PYOCD_DIR%"

echo.
echo Flashing...
pyocd.exe flash --target=%TARGET% --erase=auto --frequency=%FREQ% "%ELF_FILE%"

if %ERRORLEVEL% equ 0 (
    echo.
    echo [SUCCESS] Flash completed successfully!
) else (
    echo.
    echo [ERROR] Flash failed with error code %ERRORLEVEL%
)

echo.
echo Press any key to exit...
pause > nul

