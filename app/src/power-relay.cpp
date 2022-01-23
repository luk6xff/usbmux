#include "power-relay.h"
#include "utils.h"

//------------------------------------------------------------------------------
PowerRelay::PowerRelay(int onOffPin)
    : m_onOffPin(onOffPin)
{
    // Set Relay on/off pin
    pinMode(m_onOffPin, OUTPUT);
    digitalWrite(m_onOffPin, RELAY_OFF);
}

//------------------------------------------------------------------------------
void PowerRelay::enable(RelayState state)
{
    digitalWrite(m_onOffPin, state);
}

//------------------------------------------------------------------------------
const PowerRelay::RelayState PowerRelay::state() const
{
    return (digitalRead(m_onOffPin) == RELAY_ON) ? RELAY_ON : RELAY_OFF;
}

//------------------------------------------------------------------------------
