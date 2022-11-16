#pragma once

#include <Arduino.h>

#include <vector>
#include <memory>
#include <string>

#include "usbmuxdriver.h"
#include "power-relay.h"


///< Forward declaration of Actual Command classes
class CommandMsg;
class CmdGetPwrRelayMsg;
class CmdSetUsbChannelMsg;
class CmdSetPwrRelayMsg;
class CmdSetWifiConfigMsg;
class CmdDeviceInfoMsg;
class CmdDeviceResetMsg;
class CmdDeviceSetNameMsg;

/**
 * @brief Command Handler class.
 */
class CmdHandler
{
public:
    std::unique_ptr<std::vector<PowerRelay>> m_pwrRelays;
    explicit CmdHandler();
    void handle(CmdSetUsbChannelMsg& msg);
    void handle(CmdGetPwrRelayMsg& msg);
    void handle(CmdSetPwrRelayMsg& msg);
    void handle(CmdSetWifiConfigMsg& msg);
    void handle(CmdDeviceInfoMsg& msg);
    void handle(CmdDeviceSetNameMsg& msg);
    void handle(CmdDeviceResetMsg& msg);
    void handle(CommandMsg& msg);

private:
    std::unique_ptr<UsbMuxDriver> m_usbMux;

};


/**
 * @brief Command message interface class.
 */
class CommandMsg
{

public:
    void dispatch(CmdHandler& handler)
    {
        dispatchImpl(handler);
    }

protected:
    virtual void dispatchImpl(CmdHandler& handler) = 0;
};


/**
 * @brief Command message base class.
 * @see https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Curiously_Recurring_Template_Pattern
 */
template <typename TDerived>
class CommandMsgBase : public CommandMsg
{

protected:

    virtual void dispatchImpl(CmdHandler& handler) override
    {
        handler.handle(static_cast<TDerived&>(*this));
    }

};


////////////////////////////////////////////////////////////////////////////////
///< Actual Command classes
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
class CmdSetUsbChannelMsg : public CommandMsgBase<CmdSetUsbChannelMsg>
{

public:
    CmdSetUsbChannelMsg(UsbMuxDriver::UsbChannelNumber chNum=UsbMuxDriver::UsbChannelNumber::USB_CHANNEL_INVALID,
                     UsbMuxDriver::UsbIdState idState=UsbMuxDriver::UsbIdState::USB_ID_HIGH,
                     bool disable=false)
        : channelNumber(chNum)
        , usbIdState(idState)
        , disableChannels(disable)
    {
    }

public:
    UsbMuxDriver::UsbChannelNumber channelNumber;
    UsbMuxDriver::UsbIdState usbIdState;
    bool disableChannels;
};
//------------------------------------------------------------------------------
class CmdGetPwrRelayMsg : public CommandMsgBase<CmdGetPwrRelayMsg>
{
public:
    CmdGetPwrRelayMsg()
        :  relayId(0)
    {
    }
    CmdGetPwrRelayMsg(const uint8_t relayId)
        : relayId(relayId)
    {
    }
public:
    uint8_t relayId;
};


//------------------------------------------------------------------------------
class CmdSetPwrRelayMsg : public CommandMsgBase<CmdSetPwrRelayMsg>
{

public:
    CmdSetPwrRelayMsg()
        : relayId(0)
        , relayState(PowerRelay::RelayState::RELAY_OFF)
        , reset(false)
        , resetTimeoutMs(k_defaultTimeoutMs)
    {
    }

    CmdSetPwrRelayMsg(const uint8_t relayId, PowerRelay::RelayState state, bool reset=false, uint32_t resetTime=k_defaultTimeoutMs)
        : relayId(relayId)
        , relayState(state)
        , reset(reset)
        , resetTimeoutMs(resetTime)
    {
    }

public:
    uint8_t relayId;
    PowerRelay::RelayState relayState;
    bool reset;
    uint32_t resetTimeoutMs;

    static int const k_defaultTimeoutMs = 1000;
};


//------------------------------------------------------------------------------
class CmdSetWifiConfigMsg : public CommandMsgBase<CmdSetWifiConfigMsg>
{
public:
    CmdSetWifiConfigMsg(uint8_t id=0, String ssid="", String pass="",
                        bool read=false, bool reconnect=false)
    : wifiId(id)
    , wifiSsid(ssid)
    , wifiPass(pass)
    , wifiRead(read)
    , wifiReconnect(reconnect)
    {
    }

public:
    uint8_t wifiId;
    String wifiSsid;
    String wifiPass;
    bool wifiRead;
    bool wifiReconnect;
};


//------------------------------------------------------------------------------
class CmdDeviceInfoMsg : public CommandMsgBase<CmdDeviceInfoMsg>
{
public:
    CmdDeviceInfoMsg() = default;
};


//------------------------------------------------------------------------------
class CmdDeviceSetNameMsg : public CommandMsgBase<CmdDeviceSetNameMsg>
{
public:
    CmdDeviceSetNameMsg(String name)
    : m_name(name)
    {
    }
    String m_name;
};


//------------------------------------------------------------------------------
class CmdDeviceResetMsg : public CommandMsgBase<CmdDeviceResetMsg>
{
public:
    CmdDeviceResetMsg() = default;
};



//------------------------------------------------------------------------------
/**
 * @brief A place where all the device commands are being dispatched and run :)
 */
class Commander
{

public:
    CmdHandler m_cmdHandler;
    /**
     * @brief Ctor
     */
    explicit Commander();

    void processCmdMsg(CommandMsg& msg);

private:


private:

};
