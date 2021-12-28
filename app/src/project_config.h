#pragma once

/**
 * @brief USBMUX project configuration.
 */

#define USBMUX_ON
#define PWR_RELAYS_ON
#define DEV_SERVER_ON


// Disable USBMUX driver
#undef USBMUX_ON
// Disable DEV_WEB_SERVER
#undef DEV_SERVER_ON


