#pragma once

#include <Arduino.h>

#include <map>
#include <vector>
#include "CommandHandler.h"
#include "commander.h"

using SerialCmdMap = std::map<String, std::function<void(void)>>;
/**
 * @brief Command Handler class.
 */
class SerialCmdHandler : public CommandHandler
{
public:
    explicit SerialCmdHandler(Commander& cmdr);

    void process();

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
    Commander& m_cmdr;
    SerialCmdMap m_commands;
};

