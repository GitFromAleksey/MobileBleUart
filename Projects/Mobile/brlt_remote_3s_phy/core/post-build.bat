set APPO=%1.out
set APPH=%1.hex
set PRGH=%2\brc.hex
set SDVH=%3.hex


set MERH="C:\Program Files (x86)\Nordic Semiconductor\nrf-command-line-tools\bin\mergehex.exe"

:: calculate checksum of the application
ielftool --fill 0xFF;__vector_table-SYSTEM_AppCRC --checksum SYSTEM_AppFirmInfoCRC:2,crc16,0xFFFF;SYSTEM_AppFirmInfo-SYSTEM_AppFirmInfoCRC --verbose %APPO% %APPO%
ielftool --checksum SYSTEM_AppCRC:2,crc16,0xFFFF;__vector_table-SYSTEM_AppCRC --verbose %APPO% %APPO%

:: generate additional ouput: intel hex
ielftool.exe --ihex --verbose %APPO% %APPH%

:: generate common hex
%MERH% -m %APPH% %SDVH% -o %PRGH%


