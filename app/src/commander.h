#pragma once

#include <Arduino.h>

#include <vector>

#include "usbmuxdriver.h"
#include "power-relay.h"





///< Forward declaration of Actual Command classes
class CommandMsg;
class CmdSetChannelMsg;
class CmdSetRelayMsg;
class CmdSetWifiConfigMsg;




/**
 * @brief Command Handler class.
 */
class CmdHandler
{
public:
    explicit CmdHandler();
    void handle(CmdSetChannelMsg& msg);
    void handle(CmdSetRelayMsg& msg);
    void handle(CmdSetWifiConfigMsg& msg);
    void handle(CommandMsg& msg);

private:
    UsbMuxDriver m_usbMux;
    PowerRelay m_pwrRelay;
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
class CmdSetChannelMsg : public CommandMsgBase<CmdSetChannelMsg>
{

public:
    CmdSetChannelMsg(UsbMuxDriver::UsbChannelNumber chNum,
                     UsbMuxDriver::UsbIdState idState=UsbMuxDriver::UsbIdState::USB_ID_HIGH)
        : channelNumber(chNum)
        , usbIdState(idState)
    {
    }

public:
    UsbMuxDriver::UsbChannelNumber channelNumber;
    UsbMuxDriver::UsbIdState usbIdState; 
};


//------------------------------------------------------------------------------
class CmdSetRelayMsg : public CommandMsgBase<CmdSetRelayMsg>
{

public:
    CmdSetRelayMsg()
        : relayState(PowerRelay::RelayState::RELAY_OFF)
        , reset(false)
        , resetTimeoutMs(k_defaultTimeoutMs)
    {
    }

    CmdSetRelayMsg (PowerRelay::RelayState state, bool reset=false, uint32_t resetTime=k_defaultTimeoutMs)
        : relayState(state)
        , reset(reset)
        , resetTimeoutMs(resetTime)
    {
    }

public:
    PowerRelay::RelayState relayState;
    bool reset;
    uint32_t resetTimeoutMs;

    static int const k_defaultTimeoutMs = 1000;
};


//------------------------------------------------------------------------------
class CmdSetWifiConfigMsg : public CommandMsgBase<CmdSetWifiConfigMsg>
{

};




//------------------------------------------------------------------------------
/**
 * @brief A place where all the device commands are being dispatched and run :)
 */
class Commander
{

public:

    /**
     * @brief Ctor
     */
    explicit Commander();

    void processCmdMsg(CommandMsg& msg);

private:


private:
    CmdHandler m_cmdHandler;
};
