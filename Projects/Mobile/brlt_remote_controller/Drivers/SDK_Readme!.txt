This is library from Nordic SDK 13.1
If you use IAR 8, you need to patch this SDK.
Chage the next files from folder "patch":
 - SDK_folder/components/drivers_nrf/delay/nrf_delay.h
 - SDK_folder/components/libraries/hardfault/nrf52/handler/hardfault_handler_iar.c
 - SDK_folder/components/libraries/uart/retarget.c
 - SDK_folder/external/freertos/portable/IAR/nrf52/port.c

More info:
https://devzone.nordicsemi.com/question/135670/iar-811-error/