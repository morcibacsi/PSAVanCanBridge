@echo off
if "%1"=="" (
    echo COM port was not supplied.
    goto :eof
)
C:\Utils\esptool-win64\esptool.exe --chip esp32 --port %1 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode keep --flash_freq keep --flash_size keep 0x1000 firmware/bootloader.bin 0x8000 firmware/partitions.bin 0xe000 firmware/ota_data_initial.bin 0x10000 firmware/firmware.bin
