#include "serial-cmdhandler.h"
#include "app-settings.h"
#include "utils.h"
//------------------------------------------------------------------------------
#define SERIAL_CMD_HANDLER_DELIM ","
#define SERIAL_CMD_HANDLER_TERM1 '\n' // LF
#define SERIAL_CMD_HANDLER_TERM2 '\r' // CR
#define SERIAL_CMD_HANDLER_TERM3 '[A' // arrowup
#define SERIAL_CMD_HANDLER_TERM4 '\b' // backspace
#define SERIAL_CMD_HANDLER_TERM5 '[B' // arrowdown
//------------------------------------------------------------------------------
SerialCmdHandler::SerialCmdHandler(Commander &cmdr)
    : CommandHandler(SERIAL_CMD_HANDLER_DELIM, SERIAL_CMD_HANDLER_TERM1, SERIAL_CMD_HANDLER_TERM2, SERIAL_CMD_HANDLER_TERM3, SERIAL_CMD_HANDLER_TERM4, SERIAL_CMD_HANDLER_TERM5), m_cmdr(cmdr)
{
    // Setup callbacks for SerialCommand commands
    setCommands();
    for (auto &cmd : m_commands)
    {
        addCommand(cmd.first.c_str(), cmd.second);
    }
    setDefaultHandler(std::bind(&SerialCmdHandler::processCmdUnrecognized, this));
    addCommand("st", std::bind(&SerialCmdHandler::processCmdRelayGetSate, this));
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
        {"h", std::bind(&SerialCmdHandler::cmdMenu, this)},
        {"ch", std::bind(&SerialCmdHandler::processCmdUsbChannel, this)},
        {"pwr", std::bind(&SerialCmdHandler::processCmdPower, this)},
        {"wf", std::bind(&SerialCmdHandler::processCmdWifi, this)},
        {"inf", std::bind(&SerialCmdHandler::processCmdInfo, this)},
        {"r", std::bind(&SerialCmdHandler::processCmdReset, this)},
        {"n", std::bind(&SerialCmdHandler::processCmdSetName, this)}};
}
//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdRelayGetSate()
{
    CmdGetPwrRelayMsg msg;
    const uint8_t relayId = readIntArg();
    if (!argOk)
    {
        err("\033[1;31m No PowerRelayID applied! \033[1;39m");
        return;
    }
    msg.relayId = relayId;
    m_cmdr.processCmdMsg(msg);
}
//------------------------------------------------------------------------------
void SerialCmdHandler::cmdMenu(void)
{
    err("\033[1;36m	>>>>>>>>>>>>>>> USBMUX(POWER-RELAYS) by luk6xff (2022) <<<<<<<<<<<<<<< \033[1;39m	");
    err("");
    err("\033[1;33m	Options:\033[1;39m");
    err("\033[1;33m h                    \033[0;37m	Print the help message\033[1;39m");
    err("\033[1;33m pwr,id,[x,][r,(y)]   \033[0;37m	Set power relay state:\033[1;39m");
    err("                          \033[0;37m	id-RelayID number(0,1,2...) \033[1;39m");
    err("                          \033[0;37m	x-Off/On(0-1); r-reset; y-reset timeout value\033[1;39m");
    err("\033[1;33m inf                  \033[0;37m	Print device information data\033[1;39m");
    err("\033[1;33m n,name               \033[0;37m	Change name of the USBMUX:\033[1;39m");
    err("                          \033[0;37m	name-new name string value presented in inf command\033[1;39m");
    err("                              \033[0;37m	-maximum 20 characters\033[1;39m");
    err("\033[1;33m r                    \033[0;37m	Reboot the device\033[1;39m");
    err("\033[1;33m st,id   \033[0;37m	Checks power relay state:\033[1;39m");
    err("                          \033[0;37m	id-RelayID number(0,1,2...) \033[1;39m");
    err("\033[1;36m	>>>>>>>>>>>>>>> USBMUX(POWER-RELAYS) by luk6xff (2022) <<<<<<<<<<<<<<< \033[1;39m	");
    err("\n");
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdUsbChannel()
{

    CmdSetUsbChannelMsg msg;

    err("\033[1;31m	USBMUX Channel command not supported! \033[1;39m	");
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
            err("\033[1;31m No USBMUX Channel number argument \033[1;39m");
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
            err("\033[1;31m No USBMUX usb_id second argument \033[1;39m");
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
        err("\033[1;31m No PowerRelayID applied! \033[1;39m");
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
            inf("\033[1;31m No PowerRelay[id:%d] timeout argument applied \033[1;39m", relayId);
        }
    }
    else
    {
        const bool pwrRelayState = readIntArg();
        if (argOk)
        {
            msg.relayState = (pwrRelayState == false) ? PowerRelay::RelayState::RELAY_OFF : PowerRelay::RelayState::RELAY_ON;
            inf("PowerRelay[id:%d] state SET to: %s\r\n", relayId, pwrRelayState == false ? "RELAY_OFF" : "RELAY_ON");
        }
        else
        {
            wrn("\033[1;31m No PowerRelay argument applied \033[1;39m");
            return;
        }
    }

    m_cmdr.processCmdMsg(msg);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdWifi()
{
    CmdSetWifiConfigMsg msg;

    const uint8_t wifiId = readIntArg();
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
                err("\033[1;31m No WifiPass argument applied \033[1;39m");
                return;
            }
        }
        else
        {
            err("\033[1;31m No WifiSsid argument applied \033[1;39m");
            return;
        }
    }
    else
    {
        err("\033[1;31m No WifiId argument applied \033[1;39m");
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
        name = name.substring(0, NAME_LEN);
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
    wrn("\033[1;31m Non recognized USBMUX command \033[1;39m");
}

//------------------------------------------------------------------------------
