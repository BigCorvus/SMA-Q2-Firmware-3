# SMA-Q2-Firmware-3
3rd version for a SMA-Q2 Arduino firmware based on the sandeepmistry nrf5 core and the pfod_lp_nrf52.zip low power core mods
see https://www.forward.com.au/pfod/BLE/LowPower/index.html for more info

Implements basic watch functionality with over 1 week always on and advertising standby time.
Basic menue structure with "Apps" is implemented. Since the pfod low power core does not implement interrupts, we have to poll the buttons. The lp_comparator does not work for me.
Press the DFU bootloader combo inside the watch screen.
In order to set date and time send a string in the format yyyymmddhhmmss while you're in the main menu.


Preparation

1. install the sandeepmistry nrf5 core
2. replace the hardware folder inside C:\Users\yourComputer\AppData\Local\Arduino15\packages\sandeepmistry with the provided one
3. put the entire sketch folder into your sktch directory

If you want to experiment with the precomiled lib for the HR sensor (compiles but does not work yet, also raises the default power consumption from 50µA to 500µA):
Source: https://os.mbed.com/users/pixus_mbed/code/PixArt_PAH8011_HeartRate_nRF52/
1. put the folder PAH8002HRmon into C:\Users\yourComputer\AppData\Local\Arduino15\packages\sandeepmistry\hardware\nRF5\0.6.0\libraries
3. put the content of the PAH8002driver folder into the sketch directory
4. uncomment the necessary lines in the SMAQ2_FW3.ino main sketch file (the includes, the PAH8002HRmon HRsensor; line, Wire.h stuff......)
5. inside C:\Users\yourComputer\AppData\Local\Arduino15\packages\sandeepmistry\hardware\nRF5\0.6.0 replace the original platform.txt file 
with the "platform [for precompiled libs]].txt" naming it accordingly or uncomment the two necessary lines inside the original file...
6. try to get it up and running

The following tricks accelerate your development cycles (the necessary nrfutil.exe is included):
press ctrl + alt + 'S' to compile and export hex
C:\Users\yourComputer\Documents\Arduino\SMAQ2_FW3\nrfutil dfu genpkg --application C:\Users\yourComputer\Documents\Arduino\SMAQ2_FW3\SMAQ2_FW3.ino.SMA_Q2.hex --sd-req 0x88 C:\Users\yourComputer\Documents\Arduino\SMAQ2_FW3\SMAQ2_FW3.zip
if you have http://bluetoothinstaller.com/bluetooth-command-line-tools/btobex.html installed:  btobex -n "Galaxy S5" "C:\Users\yourComputer\Documents\Arduino\SMAQ2_FW3\SMAQ2_FW3.zip"
