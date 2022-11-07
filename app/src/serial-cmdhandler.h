#pragma once

#include <Arduino.h>

#include <map>
#include "bufor.h"
#include "CommandHandler.h"
#include "commander.h"

using SerialCmdMap = std::map<String, std::function<void(void)>>;
/**
 * @brief Command Handler class.
 */
class SerialCmdHandler : public CommandHandler
{
public:
    explicit SerialCmdHandler(Commander &cmdr);

    void process();
    void AddBuforMemory();

private:
    void setCommands();
    void cmdMenu();
    void bufScroll();

    void processCmdUsbChannel();
    void processCmdPower();
    void processCmdWifi();
    void processCmdInfo();
    void processCmdSetName();
    void processCmdReset();
    void processCmdUnrecognized();

private:
    Commander &m_cmdr;
    buforCC buforCmd;
    SerialCmdMap m_commands;
};
