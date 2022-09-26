#pragma once

/**
 * @brief USBMUX project - Hardware Pin configuration config file.
 */

#include "project_config.h"

#ifdef USBMUX_ON
    // USB multiplexer - TS3USB221
    #define USBMUX_OE_PIN   D5
    #define USBMUX_S_PIN    D6
    // USB_ID
    #define USB_ID_PIN      D0
#endif //USBMUX_ON


#ifdef PWR_RELAYS_ON
    // RELAY
    #define RELAY_IN_PIN_0  D1
    #define RELAY_IN_PIN_1  D2
    #define RELAY_IN_PIN_2  D3
    #define RELAY_IN_PIN_3  D4
    #define RELAY_IN_PIN_4  D5
    #define RELAY_IN_PIN_5  D6
    #define RELAY_IN_PIN_6  D7
    #define RELAY_IN_PIN_7  D8
#endif //PWR_RELAYS_ON
