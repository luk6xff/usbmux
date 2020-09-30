
#pragma once

#include <Arduino.h>


class PowerRelay
{

public:

    typedef enum
    {
        RELAY_ON  = LOW,
        RELAY_OFF = HIGH,
    } RelayState;

public:

    /**
     * @brief Ctor
     *
     * @param onOffPin - onOffPin
     */
    explicit PowerRelay(int onOffPin);

    /**
     * @brief Enables/Disables relay based on passed state
     *
     * @param state - relay state (on/off)
     */
    void enable(RelayState state);


    /**
     * @brief Get current relay pin state
     *
     * @param state - relay state (on/off)
     */
    const RelayState state() const;

private:

    /**
     * @brief Relay on/off pin
     */
    int m_onOffPin;
};
