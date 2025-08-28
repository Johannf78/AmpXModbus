@echo off
echo Downloading spiffsgen.py...
powershell -Command "Invoke-WebRequest -Uri 'https://raw.githubusercontent.com/espressif/esp-idf/master/components/spiffs/spiffsgen.py' -OutFile 'spiffsgen.py'"

echo Creating SPIFFS image (1.5MB - block aligned)...
python spiffsgen.py 188416 data spiffs.bin

echo SPIFFS image created: spiffs.bin
echo File size:
dir spiffs.bin
pause