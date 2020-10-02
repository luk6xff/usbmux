#include "app-settings.h"
#include <ESP_EEPROM.h>
#include "utils.h"

//------------------------------------------------------------------------------
AppSettings::AppSettings()
{
    // Modify according to your application
    defaultSettings =
    {
        .magic = 0x4C554B36,  // LUK6
        .version = 0x00000001,
    };

    WifiSettings wifiDefault;
    memcpy(wifiDefault.ssid, "admin", strlen("admin"));
    memcpy(wifiDefault.pass, "admin", strlen("admin"));
    //memcpy(&defaultSettings.wifi0, &wifiDefault, sizeof(wifiDefault));
    //memcpy(&defaultSettings.wifi1, &wifiDefault, sizeof(wifiDefault));
    //memcpy(&defaultSettings.wifi2, &wifiDefault, sizeof(wifiDefault));
    defaultSettings.wifi0 = wifiDefault;
    defaultSettings.wifi1 = wifiDefault;
    defaultSettings.wifi2 = wifiDefault;
}


//------------------------------------------------------------------------------
AppSettings&  AppSettings::instance()
{
    static AppSettings settings;
    return settings;
}

//------------------------------------------------------------------------------
void AppSettings::init()
{
    // The begin() call is required to initialise the EEPROM library
    EEPROM.begin(sizeof(Settings));

    readSettings();

    // Print content
    printCurrentSettings();

    // Check if settings are valid
    if (getCurrent().magic == getDefaults().magic && \
        getCurrent().version == getDefaults().version)
    {
        inf("APP_SETTINGS: Read eeprom settings look ok\r\n");
    }
    else
    {
        inf("APP_SETTINGS: Read eeprom settings are invalid, updating with defaults\r\n");
        if (!saveSettings(getDefaults()))
        {
            inf("APP_SETTINGS: Updating eeprom with defaults failed!, setting current as defaults\r\n");
            currentSettings = defaultSettings;
        }
        else
        {
            inf("APP_SETTINGS: Updating eeprom with defaults succeed!\r\n");
        }
    }
}

//------------------------------------------------------------------------------
const AppSettings::Settings& AppSettings::getDefaults()
{
    return defaultSettings;
}

//------------------------------------------------------------------------------
const AppSettings::Settings& AppSettings::getCurrent()
{
    return currentSettings;
}


//------------------------------------------------------------------------------
bool AppSettings::storeWifiData(uint8_t wifiSettingsNum, AppSettings::WifiSettings& ws)
{
    bool ret = true;
    Settings newSettings = getCurrent();

    if (wifiSettingsNum == 0)
    {
        newSettings.wifi0 = ws;
        ret = saveSettings(newSettings);
    }
    else if (wifiSettingsNum == 1)
    {
        newSettings.wifi1 = ws;
        ret = saveSettings(newSettings);
    }
    else if (wifiSettingsNum == 2)
    {
        newSettings.wifi2 = ws;
        ret = saveSettings(newSettings); 
    }
    else
    {
        ret = false;
    }
    return ret;
}

//------------------------------------------------------------------------------
bool AppSettings::saveSettings(const Settings &settings)
{
    // Set the EEPROM data ready for writing
    EEPROM.put(0, settings);
  
    // Write the data to EEPROM
    if (EEPROM.commit())
    {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
void AppSettings::readSettings()
{
   currentSettings = EEPROM.get(0, currentSettings);
}

//------------------------------------------------------------------------------
void AppSettings::printCurrentSettings()
{
    inf("APP_SETTINGS: <<CURRENT APP SETTINGS>>\r\n");
    inf("magic: 0x%08x\r\n", getCurrent().magic);
    inf("version: 0x%08x\r\n", getCurrent().version);
    inf("wifi0.ssid: %s\r\n", getCurrent().wifi0.ssid);
    inf("wifi0.pass: %s\r\n", getCurrent().wifi0.pass);
    inf("wifi1.ssid: %s\r\n", getCurrent().wifi1.ssid);
    inf("wifi1.pass: %s\r\n", getCurrent().wifi1.pass);
    inf("wifi2.ssid: %s\r\n", getCurrent().wifi2.ssid);
    inf("wifi2.pass: %s\r\n", getCurrent().wifi2.pass);
    inf("APP_SETTINGS: <<CURRENT APP SETTINGS>>\r\n\r\n");
}

//------------------------------------------------------------------------------