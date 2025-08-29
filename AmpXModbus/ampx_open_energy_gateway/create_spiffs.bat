@echo off
echo Downloading spiffsgen.py...
powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/espressif/esp-idf/master/components/spiffs/spiffsgen.py' -OutFile 'spiffsgen.py'"

echo Creating SPIFFS image (188KB - ESP32 optimized)...
python spiffsgen.py --page-size 256 --block-size 4096 --obj-name-len 32 --meta-len 4 --use-magic --use-magic-len 188416 data spiffs.bin

echo SPIFFS image created: spiffs.bin
echo File size:
dir spiffs.bin
pause