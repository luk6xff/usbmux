#pragma once

#include <Arduino.h>

#include <map>
#include "CommandHandler.h"
#include "commander.h"
#include <deque>

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
    std::deque<char*> deque_test;
    int deque_index = 0;

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
    SerialCmdMap m_commands;
};
