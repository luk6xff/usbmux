#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>


class DevServer
{


public:

    /**
     * @brief Ctor
     */
    explicit DevServer();

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

    /**
     * @brief Relay on/off pin
     */
    String ssid;
    String password;

    ESP8266WiFiMulti wifiMulti;
    ESP8266WebServer server;
    FS *fileSystem;
    LittleFSConfig fileSystemConfig;

    static const char *k_hostName;
};
