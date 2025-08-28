@echo off
set /p PORT="Enter COM port (e.g., COM7): "

set ESPTOOL="C:\Users\Johann\AppData\Local\Arduino15\packages\esp32\tools\esptool_py\5.0.0\esptool.exe"

echo Using esptool: %ESPTOOL%
echo Uploading SPIFFS to COM%PORT%...
%ESPTOOL% write-flash --flash-mode dio --flash-size 4MB 0x210000 spiffs.bin

echo Upload complete!
pause