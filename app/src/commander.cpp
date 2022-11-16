#include "commander.h"
#include "app-settings.h"
#include "hw_config.h"
#include "Esp.h"
#include "device-info.h"
#include "utils.h"



//------------------------------------------------------------------------------
CmdHandler::CmdHandler()
    : m_usbMux(nullptr)
    , m_pwrRelays(nullptr)
{
    dbg(__func__);

#ifdef USBMUX_ON
    // Create UsbMux
    m_usbMux = std::make_unique<UsbMuxDriver>(USB_ID_PIN, USBMUX_OE_PIN, USBMUX_S_PIN);
#endif

#ifdef PWR_RELAYS_ON
    // Create PowerRelays
    m_pwrRelays = std::make_unique<std::vector<PowerRelay>>(
        std::vector{
            PowerRelay(RELAY_IN_PIN_0),
            PowerRelay(RELAY_IN_PIN_1),
            PowerRelay(RELAY_IN_PIN_2),
            PowerRelay(RELAY_IN_PIN_3),
            PowerRelay(RELAY_IN_PIN_4),
            PowerRelay(RELAY_IN_PIN_5),
            PowerRelay(RELAY_IN_PIN_6),
            PowerRelay(RELAY_IN_PIN_7)
        }
    );
#endif
}

//------------------------------------------------------------------------------
void CmdHandler::handle(CmdSetUsbChannelMsg& msg)
{
    dbg("CmdSetUsbChannelMsg handler: %s\r\n", __func__);

    if (!m_usbMux)
    {
        err("UsbChannelMux object has not been created, exiting...");
    }
    else
    {
        if (msg.disableChannels)
        {
            m_usbMux->disableAll();
        }
        else
        {
            m_usbMux->enableChannel(msg.channelNumber, msg.usbIdState);
        }
    }
}
//------------------------------------------------------------------------------
void CmdHandler::handle(CmdGetPwrRelayMsg& msg)
{
    if (!m_pwrRelays)
    {
        err("PowerRelays object has not been created, exiting...");
    }
    else
    {
        // Check if ID is valid ?
        if (msg.relayId >= m_pwrRelays->size())
        {
            err("PwrRelayId value must be between (0-%d) !!!\r\n", m_pwrRelays->size()-1);
        }  
        else
        {
            // Extract a PwrRelay reference.
            auto& pwrRelay = (*m_pwrRelays)[msg.relayId];
            const PowerRelay::RelayState m_state = pwrRelay.state();
            if (m_state==0)
            {
                inf("PowerRelay (id:%d) state SET to: RELAY_OFF", msg.relayId);
            }
            else
            {
                inf("PowerRelay (id:%d) state SET to: RELAY_ON", msg.relayId);
            }  
        }
    }
}
//------------------------------------------------------------------------------
void CmdHandler::handle(CmdSetPwrRelayMsg& msg)
{
    dbg("CmdSetPwrRelayMsg handler: %s\r\n", __func__);

    if (!m_pwrRelays)
    {
        err("PowerRelays object has not been created, exiting...");
    }
    else
    {
        // Check if ID is valid ?
        if (msg.relayId >= m_pwrRelays->size())
        {
            err("PwrRelayId value must be between (0-%d) !!!\r\n", m_pwrRelays->size()-1);
        }
        else
        {
            // Extract a PwrRelay reference.
            auto& pwrRelay = (*m_pwrRelays)[msg.relayId];
            // Is it a reset request ?
            if (msg.reset)
            {
                const PowerRelay::RelayState prevState = pwrRelay.state();
                const PowerRelay::RelayState state = (prevState == PowerRelay::RelayState::RELAY_OFF) ? \
                                                PowerRelay::RelayState::RELAY_ON : \
                                                PowerRelay::RelayState::RELAY_OFF;
                pwrRelay.enable(state);
                inf("Resetting PwrRelay (id:%d) in %d[ms]...\r\n", msg.relayId, msg.resetTimeoutMs);
                delay(msg.resetTimeoutMs);
                pwrRelay.enable(prevState);
                inf("PwrRelay (id:%d) has been reset back to state: %d\r\n", msg.relayId, prevState);
            }
            else
            {
                pwrRelay.enable(msg.relayState);
            }
        }
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
void CmdHandler::handle(CmdDeviceInfoMsg& msg)
{
    DeviceInfo info;
    inf(info.latest().c_str());
}

//------------------------------------------------------------------------------
void CmdHandler::handle(CmdDeviceSetNameMsg& msg)
{
    AppSettings::instance().storeName(msg.m_name);
}

//------------------------------------------------------------------------------
void CmdHandler::handle(CmdDeviceResetMsg& msg)
{
    inf("Device restarting...");
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
