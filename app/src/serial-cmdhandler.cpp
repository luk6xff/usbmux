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
                            {"wf",  std::bind(&SerialCmdHandler::processCmdWifi, this)}
                            };
}

//------------------------------------------------------------------------------
void SerialCmdHandler::cmdMenu(void)
{
    err("<<<<<<<<<<<<<<<<<<<< USBMUX by luk6xff (2020) >>>>>>>>>>>>>>>>>>>>>>>");
    err("");
    err("Options:");
    err("  -h, --help                 Print this help message");
    err("  -b, --baud=baudrate        Baudrate (bps) of Arduino (default 9600)");
    err("  -p, --port=serialport      Serial port Arduino is connected to");
    err("  -r, --receive              Receive string from Arduino & print it out");
    err("  -F  --flush                Flush serial port buffers for fresh reading");
    err("  -q  --quiet                Don't print out as much info");
    err("<<<<<<<<<<<<<<<<<<<< USBMUX by luk6xff (2020) >>>>>>>>>>>>>>>>>>>>>>>");
    err("\n");
}

//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdChannel()
{
    uint8_t chNum = readIntArg();

    if (argOk)
    {
        dbg("USBMUX Channel number: %d\r\n", chNum);
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
    }
    else
    {
        err("No USBMUX usb_id second argument");
        return;
    }

    // Validate the input data
    CmdSetChannelMsg msg((UsbMuxDriver::UsbChannelNumber)chNum,
                         (UsbMuxDriver::UsbIdState)usbIdState);
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

}


//------------------------------------------------------------------------------
void SerialCmdHandler::processCmdUnrecognized()
{
    wrn("Non recognized USBMUX command");
}


//------------------------------------------------------------------------------
