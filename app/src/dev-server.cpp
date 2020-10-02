
#include "dev-server.h"
#include "utils.h"
#include "app-settings.h"

//------------------------------------------------------------------------------
const char *DevServer::k_hostName   = "usbmux";
const int   DevServer::k_serverPort = 80;

//------------------------------------------------------------------------------
DevServer::DevServer(Commander& cmdr)
    : server(k_serverPort)
    , fileSystem(&LittleFS)
    , fileSystemConfig(LittleFSConfig())
    , m_cmdr(cmdr)
{
}

//------------------------------------------------------------------------------
void DevServer::init()
{
    WiFi.setAutoConnect(0);
    if (connectToAP())
    {
        inf("\nConnected to: %s\nIP address:\t %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
        if (!LittleFS.begin())
        {
            // Sth bad happened
            err("LittleFS Mount failed");
        }
        else
        {
            inf("LittleFS Mount succesfull");
        }

        server.on("/devinfotable.json", std::bind(&DevServer::handleCreateDevInfoTable, this));
        server.on("/devinfo.json",std::bind(&DevServer::handleSendDevInfo, this));
        server.on("/usbmux", std::bind(&DevServer::handleUpdateUsbMuxGpio, this));

        server.serveStatic("/js", LittleFS, "/js");
        server.serveStatic("/css", LittleFS, "/css");
        server.serveStatic("/img", LittleFS, "/img");
        server.serveStatic("/", LittleFS, "/index.html");

        // Start the mDNS responder for usbmux
        if (MDNS.begin(k_hostName))
        {
            inf("mDNS responder started");
        }
        else
        {
            err("Error setting up MDNS responder!");
        }

        server.onNotFound(std::bind(&DevServer::handleNotFound, this));

        // Start the server
        server.begin();
        inf("HTTP server started");

        // Add service to MDNS-SD
        MDNS.addService("http", "tcp", 80);
    }
    else
    {
        err("Error - Cannot connect to AP!");
    }
}

//------------------------------------------------------------------------------
void DevServer::process()
{
    MDNS.update();
    server.handleClient();
}

//------------------------------------------------------------------------------
bool DevServer::connectToAP()
{
    int tries = 0;
    bool connected = true;
    wifiMulti.addAP(AppSettings::instance().getCurrent().wifi0.ssid,
                    AppSettings::instance().getCurrent().wifi0.pass);
    wifiMulti.addAP(AppSettings::instance().getCurrent().wifi1.ssid,
                    AppSettings::instance().getCurrent().wifi1.pass);
    wifiMulti.addAP(AppSettings::instance().getCurrent().wifi2.ssid,
                    AppSettings::instance().getCurrent().wifi2.pass);

    inf("Connecting ...");
    while (wifiMulti.run() != WL_CONNECTED)
    {
        // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest network
        delay(100);
        tries++;
        inf(".");
        // 5 seconds timeout
        if (tries > 50)
        {
            err("Connecting to AP failed!!!");
            connected = false;
            break;
        }
    }

    return connected;
}

//------------------------------------------------------------------------------
void DevServer::handleNotFound()
{
    // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
    server.send(404, "text/plain", "404: Not found");
}

//------------------------------------------------------------------------------
void DevServer::handleUpdateUsbMuxGpio()
{
    String usbMuxPin = server.arg("pin");
    String usbMuxPinState = server.arg("pinstate");
    String usbIdPinState = server.arg("usbidstate");
    String success = "true";

    typedef enum
    {
        CH_0  = 0,
        CH_1  = 1,
        POWER = 2,
    } CommandId;

    const std::map<String, int> validTable =
    {
        {"ch_0",     CH_0},
        {"ch_1",     CH_1},
        {"power",    POWER},
    };
    inf("COMMAND: %s, %s, %s", usbMuxPin.c_str(), usbMuxPinState.c_str(), usbIdPinState.c_str());
    auto pinSearch = validTable.find(usbMuxPin);
    if (pinSearch != validTable.end())
    {
        if (pinSearch->second == CH_0 || pinSearch->second == CH_1)
        {
            CmdSetChannelMsg msg(UsbMuxDriver::UsbChannelNumber::USB_CHANNEL_0);
            if (pinSearch->second == CH_1)
            {
                msg.channelNumber = UsbMuxDriver::UsbChannelNumber::USB_CHANNEL_1;
            }
            
            if (usbMuxPinState == "on")
            {
                if (usbIdPinState == "true")
                {
                    msg.usbIdState = UsbMuxDriver::UsbIdState::USB_ID_HIGH;
                }
                else
                {
                    msg.usbIdState = UsbMuxDriver::UsbIdState::USB_ID_LOW;
                }
            }
            else if (usbMuxPinState == "off")
            {
                dbg("USBMUX Sleep mode");
                msg.disableChannels = true;
            }
            else
            {
                success = "false";
            }

            // Execute only on success
            if (success == "true")
            {
                m_cmdr.processCmdMsg(msg);
            }
        }
        
        else if (pinSearch->second == POWER)
        {
            CmdSetRelayMsg msg;
            if (usbMuxPinState == "off")
            {
                inf("PowerRelay disabling...");
                msg.relayState = PowerRelay::RelayState::RELAY_OFF;

            }
            else if (usbMuxPinState == "on")
            {
                inf("PowerRelay enabling...");
                // Enable relay
                msg.relayState = PowerRelay::RelayState::RELAY_ON;
            }
            else
            {
                success = "false";
            }
            
            // Execute only on success
            if (success == "true")
            {
                m_cmdr.processCmdMsg(msg);
            }
        }
    }
    else
    {
        success = "false";
        err("Invalid USBMUX command!");
    }

    String json = "{\"usbmux\":\"" + String(usbMuxPin) + "\",";
    json += "\"pinstate\":\"" + String(usbMuxPinState) + "\",";
    json += "\"usbidstate\":\"" + String(usbMuxPinState) + "\",";
    json += "\"success\":\"" + String(success) + "\"}";

    server.send(200, "application/json", json);
    inf("USBMUX GPIO pin:%s has changed to state:%s!\n", usbMuxPin.c_str(), usbMuxPinState.c_str());
}

//------------------------------------------------------------------------------
void DevServer::handleCreateDevInfoTable()
{
    dbg("handleCreateDevInfoTable called!!");
    server.send(200, "application/json", m_devInfo.createDevInfoTable());
}

//------------------------------------------------------------------------------
void DevServer::handleSendDevInfo()
{
    dbg("handleSendDevInfo called!!");
    String json;// = "{\"freeHeap\":\"" + String(devInfo.freeHeap) + "\",";
    //json += "\"freeStack\":\"" + String(devInfo.freeStack) + "\"}";
    server.send(200, "application/json", json);
}

//------------------------------------------------------------------------------
