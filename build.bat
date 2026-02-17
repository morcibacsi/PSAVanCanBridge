@echo off
setlocal enabledelayedexpansion

:: --------------------------
:: Define platforms
:: --------------------------
set "platforms=esp32_v13_can esp32_v14_can esp32_v15_can esp32c6_v16 esp32_v15_van_ulp"
set i=0
for %%P in (%platforms%) do (
    set /a i+=1
    echo !i!. %%P
    set "platform!i!=%%P"
)
set max=%i%

:: --------------------------
:: Get platform choice
:: --------------------------
if "%1"=="" goto interactive_choice
set platform=%1
rem set "platform=!platform%choice%!"
if "!platform!"=="" (
    echo Invalid platform argument! Exiting.
    pause
    exit /b 1
)
goto get_version

:interactive_choice
:choice_loop
set /p choice="Enter the number of the platform (1-%max%): "
set "platform=!platform%choice%!"
if "!platform!"=="" (
    echo Invalid choice! Please enter a number between 1 and %max%.
    goto choice_loop
)

:get_version
:: --------------------------
:: Get version
:: --------------------------
if "%2"=="" (
    set /p version="Enter the version number: "
) else (
    set version=%2
)

if "%version%"=="" (
    echo No version specified. Exiting.
    pause
    exit /b 1
)

echo.
echo Building platform: !platform!
echo Version: !version!
echo.

:: --------------------------
:: Run PlatformIO build
:: --------------------------
C:\.platformio\penv\Scripts\pio.exe run -e !platform!
if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

:: --------------------------
:: Compress the output binary
:: --------------------------
if not exist firmware (
    mkdir firmware
)

powershell Compress-Archive -Path ".pio\build\!platform!\*.bin" -DestinationPath "firmware\psa_van_can_bridge_!platform!_!version!.zip" -Force

echo.
echo Build and packaging complete!
