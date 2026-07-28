@echo off
C:\.platformio\penv\Scripts\python.exe scripts\build_firmware.py %*
exit /b %errorlevel%
