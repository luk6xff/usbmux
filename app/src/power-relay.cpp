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
