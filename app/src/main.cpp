#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LittleFS.h>

#include "utils.h"
#include "hw_config.h"
#include "usbmuxdriver.h"
#include "power-relay.h"



#define STASSID "INTEHNETY"
#define STAPSK  "Faza199490"
#define STASSID "INTEHNETY"
#define STAPSK  "Faza199490"

const char* ssid     = STASSID;
const char* password = STAPSK;
const char* host_name = "usbmux";
const char* fsName = "LittleFS";

template<typename T>
struct InfoValue
{
    InfoValue(const char *name, T value, const char *unit="", const char *icon="glyphicon-scale")
        : m_name(name)
        , m_value(String(value))
        , m_unit(unit)
        , m_glyphIcon(icon)
    {
    }

    String toJson()
    {
        String json = "{\"name\":\""+ m_name + "\",\"value\":\"" + m_value + "\",\"unit\":\"" + m_unit + "\",\"glyph\":\"" + m_glyphIcon + "\"}";
        return json;
    }

public:
    const String &name()
    {
        return m_name;
    }

    const String &val()
    {
        return m_value;
    }

    const String &unit()
    {
        return m_unit;
    }

    const String &icon()
    {
        return m_glyphIcon;
    }

private:
    String m_name;
    String m_value;
    String m_unit;
    String m_glyphIcon;
};



class DeviceInfo
{

public:
    explicit DeviceInfo()
    : buildAuthor("Build author", "luk6xff")
    , buildDate("Build date", F(__DATE__))
    , buildTime("Build time", F(__TIME__))
    , vcc("VCC", ESP.getVcc(), "V")
    , chipId("Chip ID", ESP.getChipId())
    , fullVersion("Full version", ESP.getFullVersion())
    , bootVersion("Boot version", ESP.getBootVersion())
    , cpuFreqMhz("CPU Freq", ESP.getCpuFreqMHz(), "MHz")
    , resetInfo("Last Reset info", ESP.getResetInfo())
    , flashChipRealSize("Chip ID", ESP.getFlashChipSize())
    , appSize("Application size", ESP.getSketchSize(), "Bytes")
    , freeHeap("Free Heap", ESP.getFreeHeap(), "Bytes", "glyphicon-indent-left")
    , freeStack("Free Stack", ESP.getFreeContStack(), "Bytes", "glyphicon-tint")
    {

    }

    String createDevInfoTable()
    {
        String json = "[";
        json += buildAuthor.toJson() + ","
              + buildDate.toJson() + ","
              + buildTime.toJson() + ","
              + vcc.toJson() + ","
              + chipId.toJson() + ","
              + fullVersion.toJson() + ","
              + bootVersion.toJson() + ","
              + cpuFreqMhz.toJson() + ","
              + resetInfo.toJson() + ","
              + flashChipRealSize.toJson() + ","
              + appSize.toJson() + ","
              + freeHeap.toJson() + ","
              + freeStack.toJson();
        json += "]";
        return json;
    }

    String createDynamicDevInfo()
    {
        //String json = "{\"free_heap\":\"" + String(devInfo.freeHeap) + "\",";
        //json += "\"free_stack\":\"" + String(devInfo.freeStack) + "\"}";
        String json = "{" + freeHeap.name() + ":" + freeHeap.val() + "\","
                    + freeStack.name() + ":" + freeStack.val()
                    + "}";
        return json;
    }

private:
    InfoValue<String> buildAuthor;
    InfoValue<String> buildDate;
    InfoValue<String> buildTime;
    InfoValue<uint16_t> vcc;
    InfoValue<uint32_t> chipId;
    InfoValue<String> fullVersion;
    InfoValue<uint32_t> bootVersion;
    InfoValue<uint32_t> cpuFreqMhz;
    InfoValue<String> resetInfo;
    InfoValue<uint32_t> flashChipRealSize;
    InfoValue<uint32_t> appSize;
    InfoValue<uint32_t> freeHeap;
    InfoValue<uint32_t> freeStack;
};


static FS *fileSystem = &LittleFS;
static LittleFSConfig fileSystemConfig = LittleFSConfig();
static ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
static ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

static UsbMuxDriver usbMux(USB_ID_PIN);
static PowerRelay pwrRelay(RELAY_IN_PIN);
DeviceInfo devInfo;


static void fillDeviceInfo(DeviceInfo &devInfo)
{
}


static void handleNotFound()
{
    server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}



static void updateUsbMuxGpio()
{
    String usbMuxPin = server.arg("pin");
    String usbMuxState = server.arg("state");
    String success = "true";

    const std::map<String, int> validTable =
    {
        {"OE",    0},
        {"S",     1},
        {"ID",    2},
        {"RELAY", 3},
    };

    auto pinSearch = validTable.find(usbMuxPin);
    if (pinSearch != validTable.end())
    {

        switch (pinSearch->second)
        {
            case 0:
            {
                if (usbMuxState == "high")
                {
                    DBG.println("USBMUX Sleep mode");
                    usbMux.disableAll();
                }
                else if (usbMuxState == "low")
                {
                    // Nothing
                }
                else
                {
                    success = "false";
                }
                break;
            }

            case 1:
            {
                if (usbMuxState == "low")
                {
                    DBG.println("USBMUX Channel 0 enabled");
                    // Enable channel 0
                    usbMux.enableChannel(0, UsbMuxDriver::USB_ID_HIGH);
                }
                else if (usbMuxState == "high")
                {
                    DBG.println("USBMUX Channel 1 enabled");
                    // Enable channel 1
                    usbMux.enableChannel(1, UsbMuxDriver::USB_ID_HIGH);
                }
                else
                {
                    success = "false";
                }
                break;
            }

            case 2:
            {
                int pin = USB_ID_PIN;
                if (usbMuxState == "low")
                {
                    DBG.println("USB_ID low");
                    digitalWrite(pin, HIGH);
                }
                else if (usbMuxState == "high")
                {
                    DBG.println("USB_ID high");
                    digitalWrite(pin, HIGH);
                }
                else
                {
                    success = "false";
                }
                break;
            }

            case 3:
            {
                if (usbMuxState == "low")
                {
                    DBG.println("PowerRelay disabled!");
                    // Disable relay
                    pwrRelay.enable(PowerRelay::RELAY_OFF);
                }
                else if (usbMuxState == "high")
                {
                    DBG.println("PowerRelay enabled!");
                    // Enable relay
                    pwrRelay.enable(PowerRelay::RELAY_ON);
                }
                else
                {
                    success = "false";
                }
                break;
            }

            default:
            {
                success = "false";
                break;
            }
        }
    }
    else
    {
        success = "false";
    }

    if (success == false)
    {
        DBG.println("Setting USBMUX pin value failed!");
    }


    String json = "{\"usbmux\":\"" + String(usbMuxPin) + "\",";
    json += "\"state\":\"" + String(usbMuxState) + "\",";
    json += "\"success\":\"" + String(success) + "\"}";

    server.send(200, "application/json", json);
    DBG.printf("USBMUX GPIO pin:%s has changed to state:%s!\n", usbMuxPin.c_str(), usbMuxState.c_str());
}


static void createDevInfoTable()
{
    DBG.println("createDevInfoTable called!!");
    server.send(200, "application/json", devInfo.createDevInfoTable());
}


static void sendDevInfo()
{
    DBG.println("sendDevInfo called!!");
    String json;// = "{\"freeHeap\":\"" + String(devInfo.freeHeap) + "\",";
    //json += "\"freeStack\":\"" + String(devInfo.freeStack) + "\"}";
    server.send(200, "application/json", json);
}

void setup()
{
    DBG.begin(DBG_BAUDRATE);

    wifiMulti.addAP(ssid, password);   // add Wi-Fi networks you want to connect to
    wifiMulti.addAP("ssid_from_AP_2", "your_password_for_AP_2");
    wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

    DBG.println("Connecting ...");
    while (wifiMulti.run() != WL_CONNECTED)
    {
        // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
        delay(250);
        DBG.print('.');
    }
    DBG.println('\n');
    DBG.print("Connected to ");
    DBG.println(WiFi.SSID());
    DBG.print("IP address:\t");
    DBG.println(WiFi.localIP());

    if (!LittleFS.begin())
    {
        // Sth bad happened
        DBG.println("LittleFS Mount failed");
    }
    else
    {
        DBG.println("LittleFS Mount succesfull");
    }

    server.on("/devinfotable.json", createDevInfoTable);
    server.on("/devinfo.json", sendDevInfo);
    server.on("/usbmux", updateUsbMuxGpio);

    server.serveStatic("/js", LittleFS, "/js");
    server.serveStatic("/css", LittleFS, "/css");
    server.serveStatic("/img", LittleFS, "/img");
    server.serveStatic("/", LittleFS, "/index.html");

    if (MDNS.begin(host_name))
    {
        // Start the mDNS responder for usbmux
        DBG.println("mDNS responder started");
    }
    else
    {
        DBG.println("Error setting up MDNS responder!");
    }

    server.onNotFound(handleNotFound);

    // Start the server
    server.begin();
    DBG.println("HTTP server started");
    // Add service to MDNS-SD
    MDNS.addService("http", "tcp", 80);
}


void loop()
{

    MDNS.update();
    server.handleClient();
}



