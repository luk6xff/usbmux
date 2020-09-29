
#include "dev-server.h"
#include "secret_data.h"
#include "utils.h"


const char *DevServer::k_hostName = "usbmux";

//------------------------------------------------------------------------------
DevServer::DevServer()
    : ssid(STASSID)
    , password(STAPSK)
    , server(80)
    , fileSystem(&LittleFS)
    , fileSystemConfig(LittleFSConfig())
{

    if (connectToAP())
    {
        DBG.println('\n');
        DBG.print("Connected to: ");
        DBG.println(WiFi.SSID());
        DBG.print("IP address:\t");
        DBG.println(WiFi.localIP());
    }

    if (!LittleFS.begin())
    {
        // Sth bad happened
        DBG.println("LittleFS Mount failed");
    }
    else
    {
        DBG.println("LittleFS Mount succesfull");
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
        DBG.println("mDNS responder started");
    }
    else
    {
        DBG.println("Error setting up MDNS responder!");
    }

    server.onNotFound(std::bind(&DevServer::handleNotFound, this));

    // Start the server
    server.begin();
    DBG.println("HTTP server started");

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

    DBG.println("Connecting ...");
    while (wifiMulti.run() != WL_CONNECTED)
    {
        // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest network
        delay(500);
        tries++;
        DBG.print('.');
        // 10 seconds timeout
        if (tries > 20)
        {
            DBG.print('Connecting to AP failed!!!');
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
    String usbMuxState = server.arg("state");
    String success = "true";

    // const std::map<String, int> validTable =
    // {
    //     {"OE",    0},
    //     {"S",     1},
    //     {"ID",    2},
    //     {"RELAY", 3},
    // };

    // auto pinSearch = validTable.find(usbMuxPin);
    // if (pinSearch != validTable.end())
    // {

    //     switch (pinSearch->second)
    //     {
    //         case 0:
    //         {
    //             if (usbMuxState == "high")
    //             {
    //                 DBG.println("USBMUX Sleep mode");
    //                 usbMux.disableAll();
    //             }
    //             else if (usbMuxState == "low")
    //             {
    //                 //
    //             }
    //             else
    //             {
    //                 success = "false";
    //             }
    //             break;
    //         }

    //         case 1:
    //         {
    //             if (usbMuxState == "low")
    //             {
    //                 DBG.println("USBMUX Channel 0 enabled");
    //                 // Enable channel 0
    //                 usbMux.enableChannel(0, UsbMuxDriver::USB_ID_HIGH);
    //             }
    //             else if (usbMuxState == "high")
    //             {
    //                 DBG.println("USBMUX Channel 1 enabled");
    //                 // Enable channel 1
    //                 usbMux.enableChannel(1, UsbMuxDriver::USB_ID_HIGH);
    //             }
    //             else
    //             {
    //                 success = "false";
    //             }
    //             break;
    //         }

    //         case 2:
    //         {
    //             int pin = USB_ID_PIN;
    //             if (usbMuxState == "low")
    //             {
    //                 DBG.println("USB_ID low");
    //                 digitalWrite(pin, LOW);
    //             }
    //             else if (usbMuxState == "high")
    //             {
    //                 DBG.println("USB_ID high");
    //                 digitalWrite(pin, HIGH);
    //             }
    //             else
    //             {
    //                 success = "false";
    //             }
    //             break;
    //         }

    //         case 3:
    //         {
    //             if (usbMuxState == "low")
    //             {
    //                 DBG.println("PowerRelay disabled!");
    //                 // Disable relay
    //                 pwrRelay.enable(PowerRelay::RELAY_OFF);
    //             }
    //             else if (usbMuxState == "high")
    //             {
    //                 DBG.println("PowerRelay enabled!");
    //                 // Enable relay
    //                 pwrRelay.enable(PowerRelay::RELAY_ON);
    //             }
    //             else
    //             {
    //                 success = "false";
    //             }
    //             break;
    //         }

    //         default:
    //         {
    //             success = "false";
    //             break;
    //         }
    //     }
    // }
    // else
    // {
    //     success = "false";
    // }

    // if (success == false)
    // {
    //     DBG.println("Setting USBMUX pin value failed!");
    // }


    String json = "{\"usbmux\":\"" + String(usbMuxPin) + "\",";
    json += "\"state\":\"" + String(usbMuxState) + "\",";
    json += "\"success\":\"" + String(success) + "\"}";

    server.send(200, "application/json", json);
    DBG.printf("USBMUX GPIO pin:%s has changed to state:%s!\n", usbMuxPin.c_str(), usbMuxState.c_str());
}

//------------------------------------------------------------------------------
void DevServer::handleCreateDevInfoTable()
{
    DBG.println("handleCreateDevInfoTable called!!");
    server.send(200, "application/json", ""/*devInfo.createDevInfoTable()*/);
}

//------------------------------------------------------------------------------
void DevServer::handleSendDevInfo()
{
    DBG.println("handleSendDevInfo called!!");
    String json;// = "{\"freeHeap\":\"" + String(devInfo.freeHeap) + "\",";
    //json += "\"freeStack\":\"" + String(devInfo.freeStack) + "\"}";
    server.send(200, "application/json", json);
}

//------------------------------------------------------------------------------
