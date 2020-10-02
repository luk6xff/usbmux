#include "serial-cmdhandler.h"
#include "utils.h"


//------------------------------------------------------------------------------
#define SERIAL_CMD_HANDLER_DELIM ";"
#define SERIAL_CMD_HANDLER_TERM '\r'


//------------------------------------------------------------------------------
SerialCmdHandler::SerialCmdHandler(Commander& cmdr)
    : CommandHandler(SERIAL_CMD_HANDLER_DELIM, SERIAL_CMD_HANDLER_TERM)
    , m_cmdr(cmdr)
{
    // Setup callbacks for SerialCommand commands
    setCommands();
    for (auto& cmd : m_commands)
    {
        addCommand(cmd.first.c_str(), cmd.second);
    }

    setDefaultHandler(std::bind(&SerialCmdHandler::processCmdUnrecognized, this));
    cmdMenu();
}

//------------------------------------------------------------------------------
void SerialCmdHandler::process()
{
    processSerial(Serial);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::setCommands()
{
    m_commands = SerialCmdMap{
                            {"h",   std::bind(&SerialCmdHandler::cmdMenu, this)},
                            {"ch",  std::bind(&SerialCmdHandler::processCmdChannel, this)},
                            {"pwr", std::bind(&SerialCmdHandler::processCmdPower, this)},
                            {"wf",  std::bind(&SerialCmdHandler::processCmdWifi, this)},
                            {"r",   std::bind(&SerialCmdHandler::processCmdReset, this)},
                            };
}

//------------------------------------------------------------------------------
void SerialCmdHandler::cmdMenu(void)
{
    err("<<<<<<<<<<<<<<<<<<<< USBMUX by luk6xff (2020) >>>>>>>>>>>>>>>>>>>>>>>");
    err("");
    err("Options:");
    err("  h;                   Print the help message");
    err("  ch;[[0/1];[0/1]][d]  Set usbmux channel [[0/1-Channel number][0/1-UsbID for a given channel]][d-Disable all channels]");
    err("  pwr;[0/1][r;(X)]     Set power relay state[0/1-Off/On][r-reset(X-reset timeout value)]");
    err("  wf;[X];[ssid];[pass] Set Wifi Module - Store access data for a wifi AP");
    err("  inf;                 Print device information data");
    err("  r;                   Reboot the module");
    err("<<<<<<<<<<<<<<<<<<<< USBMUX by luk6xff (2020) >>>>>>>>>>>>>>>>>>>>>>>");
    err("\n");
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdChannel()
{

    CmdSetChannelMsg msg;

    // Read command
    if (compareCheckStringArg("d"))
    {
        // Drop 'd' command
        readStringArg();
        msg.disableChannels = true;
    }
    else
    {
        uint8_t chNum = readIntArg();
        if (argOk)
        {
            dbg("USBMUX Channel number: %d\r\n", chNum);
            msg.channelNumber = (UsbMuxDriver::UsbChannelNumber)chNum;
        }
        else
        {
            err("No USBMUX Channel number argument");
            return;
        }

        bool usbIdState = readBoolArg();
        if (argOk)
        {
            dbg("USBMUX usb_id state: %d\r\n", usbIdState);
            msg.usbIdState = (UsbMuxDriver::UsbIdState)usbIdState;
        }
        else
        {
            err("No USBMUX usb_id second argument");
            return;
        }
    }

    m_cmdr.processCmdMsg(msg);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdPower() 
{
    CmdSetRelayMsg msg;

    // Read command
    if (compareCheckStringArg("r"))
    {
        // Drop 'r' command
        readStringArg();
        msg.reset = true;
        
        uint32_t timeout = readIntArg();
        if (argOk)
        {
            const uint32_t maxTimeout = 30000;
            if (timeout > maxTimeout)
            {
                inf("PowerRelay reset timeout changed to max value: %d[ms]\r\n", timeout);
                timeout = maxTimeout;
            }
            msg.resetTimeoutMs = timeout;
            dbg("PowerRelay reset timeout: %d[ms]\r\n", timeout);
        }
        else
        {
            inf("No PowerRelay timeout argument applied");
        }
        
    }
    else
    {
        const bool pwrRelayState = readIntArg();
        if (argOk)
        {
            msg.relayState = (pwrRelayState == false) ? \
                                PowerRelay::RelayState::RELAY_OFF : \
                                PowerRelay::RelayState::RELAY_ON;
            dbg("PowerRelay state: %d\r\n", pwrRelayState);
        }
        else
        {
            wrn("No PowerRelay argument applied");
            return;
        }
    }

    m_cmdr.processCmdMsg(msg);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdWifi()
{
    CmdSetWifiConfigMsg msg;
 
    const uint8_t wifiId  = readIntArg();
    if (argOk)
    {
        msg.wifiId = wifiId;
        String ssid = readStringArg();
        if (argOk)
        {
            msg.wifiSsid = ssid; 
            String pass = readStringArg();
            if (argOk)
            {
                msg.wifiPass = pass; 
            }
            else
            {
                err("No WifiPass argument applied");
                return;
            }
        }
        else
        {
            err("No WifiSsid argument applied");
            return;
        }
    }
    else
    {
        err("No WifiId argument applied");
        return;
    }
    m_cmdr.processCmdMsg(msg);
}


//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdReset()
{
    CmdDeviceResetMsg msg;
    m_cmdr.processCmdMsg(msg);
}


//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdUnrecognized()
{
    wrn("Non recognized USBMUX command");
}


//------------------------------------------------------------------------------
