# usbmux (power-relays)
Simple Power Relay controller with UART(Serial) interface.
For USB Multiplexer functionality visit [master branch](https://github.com/luk6xff/usbmux/tree/master).

## Instalation
* Install `vscode` with `platformio` plugin (Versions: vscode[1.49.2], platformio[Core:5.0.1, Home:3.3.0])


## Building and flashing

```sh
# If you use DevWebServer in your app `#define DEV_SERVER_ON in project_config.h` run:
## Build filesystem
platformio run --target buildfs --environment d1_mini
## Upload filesystem image
platformio run --target uploadfs --environment d1_mini

# And finally build and upload the application image:
## Build the application
platformio run
## Upload the application binary
platformio run --target upload

# All in one-liner
platformio run  &&  platformio run --target buildfs --environment d1_mini  &&  platformio run --target uploadfs --environment d1_mini  &&  platformio run --target upload
```

## Pinout
* HW pinout defined at: `usbmux/app/src/hw_config.h`

## Pics
* The device:
![usbmux_pwr_relays1](docs/pics/IMG1.jpg)
![usbmux_pwr_relays2](docs/pics/IMG2.jpg)
![usbmux_pwr_relays3](docs/pics/IMG3.jpg)
![usbmux_pwr_relays4](docs/pics/IMG4.jpg)
* UART(Serial) CLI:
![usbmux_pwr_relays5](docs/pics/IMG5.png)

## BOM

| PART NAME                          | SHOP LINK                                                                                        | DATASHEET | QUANTITY | PRICE [PLN] |
|------------------------------------|--------------------------------------------------------------------------------------------------|-----------|----------|-------------|
| ESP8266 WEMOS D1-MINI | [Allegro](https://allegro.pl/oferta/wemos-d1-mini-nodemcu-esp8266-12f-esp-12-arduino-9838983193?snapshot=MjAyMi0wNy0wNlQxNDoxMzoyNi4wMTFaO2J1eWVyOzU2ZWExMTFiZjVkN2JjNGE2MDYxOGJiMzMyMjI2MGM5YmQxYWUxYzIzOTdiMTgyNGZkZDgwNDk2NTIwOGQ0MDg%3D)|[ESP8266-D1MINI](./https://docs.wemos.cc/en/latest/d1/d1_mini.html) | 1 | 18.39          |
| 2-channel relay module | [Allegro](https://allegro.pl/oferta/przekaznik-5v-arduino-2-kanalowy-ark-high-low-9986689907?snapshot=MjAyMi0wNy0wNlQxNDoxMzoyNi4wMTFaO2J1eWVyOzk4MThmYzRhNDMwMjJhZjM0NmEwZDAxM2EyMzczMzcyZTlhNzIxNmI4YzdjNGI0NTExN2JmYjJkMTBkNmVlZWE%3D) | - | 1 | 14.00                                                                                 |
| Case | [Allegro](https://allegro.pl/oferta/puszka-instalacyjna-pk-4-8797267687?snapshot=MjAyMi0wNy0wNlQxNDoxMzoyNi4wMTFaO2J1eWVyO2M5YWNhNGIyNDI5NjQ5YjNhYmI5ZGFiNWM1Y2FlZGMxY2ZhMTg2MDdiMzVlZTIyMmFmZTI1NzYxYmFjZTU1ZDE%3D) | - | 1 | 8.40           |
| Electric socket | [Allegro](https://allegro.pl/oferta/gniazdo-gn-170k-10720952444?snapshot=MjAyMi0wNy0wNlQxNDoxMzoyNi4wMTFaO2J1eWVyOzNjMDc5YzU0YzBmYWZjOWFhY2JlYjQ2NzUxNTkzMTMzOWFmYjk0NWRjMGZhYzRmN2NhYmVlZWE0NDJmNzcwZmM%3D) | - | 1 | 3.20                           |
| Connecting cable with a plug | [Allegro](https://allegro.pl/oferta/przewod-przylaczeniowy-wj-22-1-5-m-7489403465?snapshot=MjAyMi0wNy0wNlQxNDoxMzoyNi4wMTFaO2J1eWVyO2QwYTlmZjFjZWY2NWMwYmViZTJmOWRmMzQzNTVkZDVjYzgwODdhM2QwYjk5NTc4NGFjNDgzYTRhODUwN2IxN2Q%3D) | - | 1 | 9.14                          |
|  |  |  |  | =~ 54.00                                                                                                                                                       |
