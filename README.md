# usbmux
USB 2.0 multiplexer based on TS3USB221 module from TI



### Pinout
Patrzac od micro usb
1. ID -  D0
2 - empt
3. oe  -D5
4. S -D6
5 gnd

RElay-ON D7

obudowa z-23


### Instalation
* Install `vscode` with `platformio` plugin (Versions: vscode[1.49.2], platformio[5.0.1])


### Building and flashing
```sh

# Build an application
platformio run

# Build filesystem
platformio run --target buildfs --environment d1_mini

# Upload filesystem image
platformio run --target uploadfs --environment d1_mini

# Upload the application binary
platformio run --target upload
```
