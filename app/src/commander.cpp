#include "commander.h"
#include "hw_config.h"
#include "utils.h"



//------------------------------------------------------------------------------
CmdHandler::CmdHandler()
    : m_usbMux(USB_ID_PIN)
    , m_pwrRelay(RELAY_IN_PIN)
{
    dbg(__func__);
}

//------------------------------------------------------------------------------
void CmdHandler::handle(CmdSetChannelMsg& msg)
{
    dbg("CmdSetChannelMsg handler: %s\r\n", __func__);
    m_usbMux.enableChannel(msg.channelNumber, msg.usbIdState);
}

//------------------------------------------------------------------------------
void CmdHandler::handle(CmdSetRelayMsg& msg)
{
    dbg("CmdSetRelayMsg handler: %s\r\n", __func__);
    // Is it a reset request ?
    if (msg.reset)
    {
        PowerRelay::RelayState prevState = m_pwrRelay.state();
        PowerRelay::RelayState state = (prevState == PowerRelay::RelayState::RELAY_OFF) ? \
                                        PowerRelay::RelayState::RELAY_ON : \
                                        PowerRelay::RelayState::RELAY_OFF;
        m_pwrRelay.enable(state);
        inf("Resetting PwrRelay in %d[ms]...\r\n", msg.resetTimeoutMs);
        delay(msg.resetTimeoutMs);
        m_pwrRelay.enable(prevState);
    }
    else
    {
        m_pwrRelay.enable(msg.relayState);
    }
}

//------------------------------------------------------------------------------
void CmdHandler::handle(CmdSetWifiConfigMsg& msg)
{
    dbg("CmdSetWifiConfigMsg handler: %s\r\n", __func__);
}

//------------------------------------------------------------------------------
void CmdHandler::handle(CommandMsg& msg)
{
    dbg("Default handler: %s\r\n", __func__);
}

//------------------------------------------------------------------------------
Commander::Commander()
{

}

//------------------------------------------------------------------------------
void Commander::processCmdMsg(CommandMsg& msg)
{
    dbg("processCmdMsg(CommandMsg& msg)");
    msg.dispatch(m_cmdHandler);
}

//------------------------------------------------------------------------------
