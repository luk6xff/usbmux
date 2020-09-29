#include "commander.h"
#include "hw_config.h"



//------------------------------------------------------------------------------
CmdHandler::CmdHandler()
    : m_usbMux(USB_ID_PIN)
    , m_pwrRelay(RELAY_IN_PIN)
{

}

//------------------------------------------------------------------------------
void CmdHandler::handle(CmdSetChannelMsg& msg)
{
    m_usbMux.enableChannel(msg.m_channelNumber, msg.m_usbIdState);
}

//------------------------------------------------------------------------------
void CmdHandler::handle(CmdSetRelayMsg& msg)
{
    m_pwrRelay.enable(msg.m_relState);   
}


//------------------------------------------------------------------------------
void CmdHandler::handle(CmdSetWifiConfigMsg& msg)
{
    
}


//------------------------------------------------------------------------------
void CmdHandler::handle(CommandMsg& msg)
{
    
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
Commander::Commander()

{

}


//------------------------------------------------------------------------------
void Commander::processCmdMsg(CommandMsg& msg)
{
    m_cmdHandler.handle(msg);
}

//------------------------------------------------------------------------------
