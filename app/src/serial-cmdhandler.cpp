#include "serial-cmdhandler.h"
#include "app-settings.h"
#include "utils.h"

//------------------------------------------------------------------------------
#define SERIAL_CMD_HANDLER_DELIM ","
#define SERIAL_CMD_HANDLER_TERM1 '\n' // LF
#define SERIAL_CMD_HANDLER_TERM2 '\r' // CR
#define SERIAL_CMD_HANDLER_BUFUP 'A' // arrowup
#define SERIAL_CMD_HANDLER_BUFDWN 'B' // arrowdown
#define SERIAL_CMD_HANDLER_RMCHR '\b' // backspace
//------------------------------------------------------------------------------
#define YELLOW "\033[1;33m", "\033[1;39m"
#define BLUE "\033[1;36m", "\033[1;39m"
#define RED "\033[1;31m", "\033[1;39m"


//------------------------------------------------------------------------------
SerialCmdHandler::SerialCmdHandler(Commander& cmdr)
    : CommandHandler(SERIAL_CMD_HANDLER_DELIM, SERIAL_CMD_HANDLER_TERM1, SERIAL_CMD_HANDLER_TERM2, SERIAL_CMD_HANDLER_BUFUP, SERIAL_CMD_HANDLER_BUFDWN, SERIAL_CMD_HANDLER_RMCHR)
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
                            {"ch",  std::bind(&SerialCmdHandler::processCmdUsbChannel, this)},
                            {"pwr", std::bind(&SerialCmdHandler::processCmdPower, this)},
                            {"wf",  std::bind(&SerialCmdHandler::processCmdWifi, this)},
                            {"inf", std::bind(&SerialCmdHandler::processCmdInfo, this)},
                            {"r",   std::bind(&SerialCmdHandler::processCmdReset, this)},
                            {"n",   std::bind(&SerialCmdHandler::processCmdSetName, this)}
                            };
}
//------------------------------------------------------------------------------
void SerialCmdHandler::cmdMenu(void)
{
    err("%s>>>>>>>>>>>>>>> USBMUX(POWER-RELAYS) by luk6xff (2022) <<<<<<<<<<<<<<<%s", BLUE);
    err("");
    err("%sOptions:%s", YELLOW);
    err("%s h           %sPrint the help message", YELLOW);
    err("%s pwr,id,[x,][r,(y)][g,]   %s	Set power relay state:", YELLOW);
    err("                      id-RelayID number(0,1,2...)");
    err("                      x-Off/On(0-1); r-reset; y-reset timeout value");
    err("                      g-get_state");
    err("%s inf         %s Print device information data", YELLOW);
    err("%s n,name      %s Change name of the USBMUX:", YELLOW);
    err("                      name:");
    err("                        -new name string value presented in inf command");
    err("                        -maximum 20 characters");
    err("%s r           %s Reboot the device", YELLOW);
    err("%s>>>>>>>>>>>>>>> USBMUX(POWER-RELAYS) by luk6xff (2022) <<<<<<<<<<<<<<<%s", BLUE);
    err("\n");
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdUsbChannel()
{

    CmdSetUsbChannelMsg msg;

    err("%sUSBMUX Channel command not supported!%s", RED);
    return;

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
            inf("USBMUX Channel number: %d\r\n", chNum);
            msg.channelNumber = (UsbMuxDriver::UsbChannelNumber)chNum;
        }
        else
        {
            err("%sNo USBMUX Channel number argument%s", RED);
            return;
        }

        bool usbIdState = readBoolArg();
        if (argOk)
        {
            inf("USBMUX usb_id state: %d\r\n", usbIdState);
            msg.usbIdState = (UsbMuxDriver::UsbIdState)usbIdState;
        }
        else
        {
            err("%sNo USBMUX usb_id second argument%s", RED);
            return;
        }
    }


    m_cmdr.processCmdMsg(msg);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdPower()
{
    CmdSetPwrRelayMsg msg;

    // Read RelayID
    const uint8_t relayId = readIntArg();
    if (!argOk)
    {
        err("%sNo PowerRelayID applied!%s", RED);
        return;
    }
    msg.relayId = relayId;

    // Is it a Reset PwrRelay command ?
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
                inf("PowerRelay[id:%d] reset timeout changed to max value: %d[ms]\r\n", relayId, timeout);
                timeout = maxTimeout;
            }
            msg.resetTimeoutMs = timeout;
            inf("PowerRelay[id:%d] reset timeout: %d[ms]\r\n", relayId, timeout);
        }
        else
        {
            err("%sNo timeout argument applied%s", RED);
        }
    }
    else if (compareCheckStringArg("g"))
    {
        //Drop 'g' command
        readStringArg();
        msg.get_state = true;
    }
    else
    {
        const bool pwrRelayState = readIntArg();
        if (argOk)
        {
            msg.relayState = (pwrRelayState == false) ? \
                            PowerRelay::RelayState::RELAY_OFF : \
                            PowerRelay::RelayState::RELAY_ON;
            inf("PowerRelay[id:%d] state SET to: %s\r\n", relayId, pwrRelayState == false ? "RELAY_OFF" : "RELAY_ON");
        }
        else
        {
            err("%sNo PowerRelay argument applied%s", RED);
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
                err("%sNo WifiPass argument applied%s", RED);
                return;
            }
        }
        else
        {
            err("%sNo WifiSsid argument applied%s", RED);
            return;
        }
    }
    else
    {
        err("%sNo WifiId argument applied%s", RED);
        return;
    }
    inf("\033[1;32m	 New Wifi AP data will be stored - channel:%d, ssid:%s, pass:%s \033[1;39m",
        msg.wifiId, msg.wifiSsid.c_str(), msg.wifiPass.c_str());
    m_cmdr.processCmdMsg(msg);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdInfo()
{
    CmdDeviceInfoMsg msg;
    m_cmdr.processCmdMsg(msg);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdSetName()
{
    // read name
    String name = readStringArg();
    if (name.length() > 19)
    {
        name = name.substring(0,NAME_LEN);
    }
    inf("Matched Name: %s", name.c_str());
    CmdDeviceSetNameMsg msg(name);
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
    wrn("%sNon recognized USBMUX command%s", RED);
}

//------------------------------------------------------------------------------
