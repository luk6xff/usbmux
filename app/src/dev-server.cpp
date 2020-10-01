
#include "dev-server.h"
#include "secret_data.h"
#include "utils.h"

//------------------------------------------------------------------------------
const char *DevServer::k_hostName   = "usbmux";
const int   DevServer::k_serverPort = 80;

//------------------------------------------------------------------------------
DevServer::DevServer(Commander& cmdr)
    : ssid(STASSID)
    , password(STAPSK)
    , server(k_serverPort)
    , fileSystem(&LittleFS)
    , fileSystemConfig(LittleFSConfig())
    , m_cmdr(cmdr)
{

}

//------------------------------------------------------------------------------
void DevServer::init()
{
    if (connectToAP())
    {
        dbg("\nConnected to: %s\nIP address:\t %s", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
    }

    if (!LittleFS.begin())
    {
        // Sth bad happened
        dbg("LittleFS Mount failed");
    }
    else
    {
        dbg("LittleFS Mount succesfull");
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
        dbg("mDNS responder started");
    }
    else
    {
        dbg("Error setting up MDNS responder!");
    }

    server.onNotFound(std::bind(&DevServer::handleNotFound, this));

    // Start the server
    server.begin();
    dbg("HTTP server started");

    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
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
    wifiMulti.addAP(ssid.c_str(), password.c_str());
    wifiMulti.addAP("ssid_ap2", "passwd_ap2");
    wifiMulti.addAP("ssid_ap3", "passwd_ap3");

    dbg("Connecting ...");
    while (wifiMulti.run() != WL_CONNECTED)
    {
        // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest network
        delay(500);
        tries++;
        dbg(".");
        // 10 seconds timeout
        if (tries > 20)
        {
            dbg("Connecting to AP failed!!!");
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
    dbg("COMMAND: %s, %s, %s", usbMuxPin.c_str(), usbMuxPinState.c_str(), usbIdPinState.c_str());
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
                dbg("PowerRelay disabling...");
                msg.relayState = PowerRelay::RelayState::RELAY_OFF;

            }
            else if (usbMuxPinState == "on")
            {
                dbg("PowerRelay enabling...");
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
    dbg("USBMUX GPIO pin:%s has changed to state:%s!\n", usbMuxPin.c_str(), usbMuxPinState.c_str());
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
