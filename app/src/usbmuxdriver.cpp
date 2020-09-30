#include "usbmuxdriver.h"
#include "utils.h"
#include "hw_config.h"

//------------------------------------------------------------------------------
UsbMuxDriver::UsbMuxDriver(int usbIdPin)
    : m_usbIdPin(usbIdPin)
{
    m_usbMuxArduinoDev.oe_pin = USBMUX_OE_PIN;
    m_usbMuxArduinoDev.s_pin  = USBMUX_S_PIN;
    ts3usb221_arduino_init(&m_usbMuxDev, &m_usbMuxArduinoDev);
    // Set USB_ID pin
    pinMode(m_usbIdPin, OUTPUT);
    digitalWrite(m_usbIdPin, USB_ID_HIGH);
}

//------------------------------------------------------------------------------
void UsbMuxDriver::enableChannel(UsbChannelNumber chNumber, UsbIdState idState)
{
    if (!isChannelValid(chNumber))
    {
        err("Invalid usb mux channel number: %d!\n", chNumber);
        return;
    }

    if (chNumber == USB_CHANNEL_0)
    {
        ts3usb221_enable_1D_channel(&m_usbMuxDev);
    }

    else if (chNumber == USB_CHANNEL_1)
    {
        ts3usb221_enable_2D_channel(&m_usbMuxDev);
    }

    // Set USB_ID pin state
    digitalWrite(m_usbIdPin, idState);

}

//------------------------------------------------------------------------------
void UsbMuxDriver::disableAll()
{
    ts3usb221_low_power_mode(&m_usbMuxDev);
    digitalWrite(m_usbIdPin, USB_ID_HIGH);
}


//------------------------------------------------------------------------------

bool UsbMuxDriver::isChannelValid(UsbChannelNumber chNumber)
{
    return (chNumber < UsbChannelNumber::USB_CHANNEL_INVALID) ? true : false;
}


//------------------------------------------------------------------------------
