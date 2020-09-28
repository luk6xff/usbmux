# usbmux
USB 2.0 multiplexer based on TS3USB221 module from TI



### Pinout
* HW pinout defined at: `usbmux/app/src/hw_config.h`
* Board connector pinout:
![usbmux_board_pinout](docs/pics/usbmux_board_pinout.jpg)




### Instalation
* Install `vscode` with `platformio` plugin (Versions: vscode[1.49.2], platformio[Core:5.0.1, Home:3.3.0])


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

### BOM
obudowa z-23