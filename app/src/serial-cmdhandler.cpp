#include "serial-cmdhandler.h"


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
    // setDefaultHandler(unrecognized);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::process()
{
    processSerial(Serial);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::setCommands()
{
    m_commands = SerialCmdMap{{"ch", std::bind(&SerialCmdHandler::processCmdChannel, this)},
                              {"pwr", std::bind(&SerialCmdHandler::processCmdPower, this)},
                              {"wf", std::bind(&SerialCmdHandler::processCmdWifi, this)}};
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdChannel()
{
    uint8_t chNum = readIntArg();

    if (argOk)
    {
        Serial.printf("USBMUX Channel number: %d\r\n", chNum);
    }
    else
    {
        Serial.println("No USBMUX Channel number arguments");
        return;
    }

    bool usbIdState = readBoolArg();
    if (argOk)
    {
        Serial.printf("USBMUX usb_id state: %d\r\n", usbIdState);
    }
    else
    {
        Serial.println("No USBMUX usb_id second argument");
        return;
    }

    CmdSetChannelMsg msg((UsbMuxDriver::UsbChannelNumber)chNum,
                        (UsbMuxDriver::UsbIdState)usbIdState);
    m_cmdr.processCmdMsg(msg);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdPower() 
{
    bool pwrRelayState = readBoolArg();
    if (argOk)
    {
        Serial.printf("PowerRelay state: %d\r\n", pwrRelayState);
    }
    else
    {
        Serial.println("No PowerRelay argument applied");
        return;
    }

    CmdSetRelayMsg msg((PowerRelay::RelayState)pwrRelayState);
    m_cmdr.processCmdMsg(msg);
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdWifi()
{

}


//------------------------------------------------------------------------------

