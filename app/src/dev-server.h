#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>
#include "commander.h"
#include "device-info.h"



class DevServer
{


public:

    /**
     * @brief Ctor
     */
    explicit DevServer(Commander& cmdr);


    /**
     * @brief DevServer init function, should be called in the setup function
     */
    void init();


    /**
     * @brief DevServer process function, should be called in the main loop
     */
    void process();

private:

    bool connectToAP();

    ///< DevServer request handlers
    /**
     * @brief Handles 404 error
     */
    void handleNotFound();

    void handleUpdateUsbMuxGpio();
    void handleCreateDevInfoTable();
    void handleSendDevInfo();

private:

    ESP8266WiFiMulti wifiMulti;
    ESP8266WebServer server;
    FS *fileSystem;
    LittleFSConfig fileSystemConfig;
    Commander& m_cmdr;
    DeviceInfo m_devInfo;

    static const char *k_hostName;
    static const int k_serverPort;
};
