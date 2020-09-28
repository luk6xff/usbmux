# usbmux
USB 2.0 multiplexer based on TS3USB221 module from TI



## Pinout
* HW pinout defined at: `usbmux/app/src/hw_config.h`
* Board connector pinout:
![usbmux_board_pinout](docs/pics/usbmux_board_pinout.jpg)


## Instalation
* Install `vscode` with `platformio` plugin (Versions: vscode[1.49.2], platformio[Core:5.0.1, Home:3.3.0])


## Building and flashing
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

## BOM
obudowa z-23

| PART NAME                          | SHOP LINK                                                                                        | DATASHEET | QUANTITY | PRICE [PLN] |
|------------------------------------|--------------------------------------------------------------------------------------------------|-----------|----------|-------------|
| ESP8266 WEMOS D1-MINI | [Allegro](https://allegro.pl/oferta/modul-wifi-d1-mini-esp8266-ex-arduino-iot-wemos-9679673735)               |[ESP8266-D1MINI](./https://docs.wemos.cc/en/latest/d1/d1_mini.html)           | 1        | 14.99       |
| TS3USB221 - USB 2.0 Mux          | [AliExpress](https://pl.aliexpress.com/item/32829759803.html) |[TS3USB221](./app/lib/TS3USB221/docs/ts3usb221.pdf)            | 1        | 5.00       |
|                                    |                                                                                                  |           |          |             |