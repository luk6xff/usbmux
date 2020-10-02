#include "commander.h"
#include "app-settings.h"
#include "hw_config.h"
#include "Esp.h"
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
    if (msg.disableChannels)
    {
        m_usbMux.disableAll();   
    }
    else
    {
        m_usbMux.enableChannel(msg.channelNumber, msg.usbIdState);
    }
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
    if (msg.wifiId >= WIFI_SETTINGS_CH_NUM)
    {
        err("Too big wifi channel number: %d\r\n", WIFI_SETTINGS_CH_NUM);
        return;
    }
    AppSettings::WifiSettings ws;
    memcpy(&ws.ssid, msg.wifiSsid.c_str(), WIFI_SETTINGS_LEN);
    memcpy(&ws.pass, msg.wifiPass.c_str(), WIFI_SETTINGS_LEN);
    AppSettings::instance().storeWifiData(msg.wifiId, ws);
}

//------------------------------------------------------------------------------
void CmdHandler::handle(CmdDeviceResetMsg& msg)
{
    ESP.restart();
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
