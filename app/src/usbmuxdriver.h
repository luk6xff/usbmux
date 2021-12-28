
#pragma once

#include <Arduino.h>
#include "ts3usb221-arduino.h"

class UsbMuxDriver
{

public:

    typedef enum
    {
        USB_ID_HIGH = HIGH,
        USB_ID_LOW  = LOW,
    } UsbIdState;

    typedef enum
    {
        USB_CHANNEL_0  = 0,
        USB_CHANNEL_1  = 1,
        USB_CHANNEL_INVALID,
    } UsbChannelNumber;

public:

    /**
     * @brief Ctor
     *
     * @param usbIdPin - USB_ID pin
     * @param usbMuxOEPin - USB_MUX_OE pin
     * @param usbMuxSPin - USB_MUX_S pin
     */
    explicit UsbMuxDriver(int usbIdPin, int usbMuxOEPin, int usbMuxSPin);

    /**
     * @brief Enables a given channel, setting proper state on usb_id pin
     *
     * @param chNumber - channel number to be enabled on the output
     * @param id       - usb id pin state enbaled on given channel
     */
    void enableChannel(UsbChannelNumber chNumber, UsbIdState idState);

    /**
     * @brief Disbles all channels, not forwarding any on the output
     *
     * @note USB ID pin state is set to LOW
     */
    void disableAll();

private:

    /**
     * @brief Returns if channel number is available (valid)
     *
     * @param chNumber - channel number to be checked
     * @return Is channel number valid (true) or not (false)
     */
    bool isChannelValid(UsbChannelNumber chNumber);

private:
    /**
     * @brief USB multiplexer dev objects
     */
    ts3usb221 m_usbMuxDev;
    ts3usb221_arduino m_usbMuxArduinoDev;

    /**
     * @brief USB_ID pin
     */
    int m_usbIdPin;
};
