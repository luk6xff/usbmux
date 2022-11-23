#include "app-settings.h"
#include <ESP_EEPROM.h>
#include "Arduino.h"
#include "utils.h"

//------------------------------------------------------------------------------
AppSettings::AppSettings()
{
    // Modify according to your application
    defaultSettings =
        {
            .magic = 0x4C554B36, // LUK6
            .version = 0x00000008,
        };

    WifiSettings wifiDefault;
    // Clear wifi default settings
    memset(&wifiDefault.ssid, 0, sizeof(wifiDefault.ssid));
    memset(&wifiDefault.pass, 0, sizeof(wifiDefault.pass));
    memcpy(wifiDefault.ssid, "admin", strlen("admin"));
    memcpy(wifiDefault.pass, "admin", strlen("admin"));
    // Assign defaults

    memset(&defaultSettings.name, 0, sizeof(defaultSettings.name));
    memcpy(&defaultSettings.name, "MUX_DEFAULT", strlen("MUX_DEFAULT"));
    defaultSettings.wifi0 = wifiDefault;
    defaultSettings.wifi1 = wifiDefault;
    defaultSettings.wifi2 = wifiDefault;
}

//------------------------------------------------------------------------------
AppSettings &AppSettings::instance()
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
    if (getCurrent().magic == getDefaults().magic &&
        getCurrent().version == getDefaults().version)
    {
        dbg("APP_SETTINGS: Read eeprom settings look ok\r\n");
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
const AppSettings::Settings &AppSettings::getDefaults()
{
    return defaultSettings;
}

//------------------------------------------------------------------------------
const AppSettings::Settings &AppSettings::getCurrent()
{
    return currentSettings;
}

//------------------------------------------------------------------------------
bool AppSettings::storeWifiData(uint8_t wifiSettingsNum, AppSettings::WifiSettings &ws)
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
bool AppSettings::storeName(const String &name_)
{
    bool ret = true;
    const char *name = name_.c_str();
    Settings newSettings = getCurrent();

    memset(&newSettings.name, 0, sizeof(newSettings.name));
    memcpy(&newSettings.name, name, strlen(name));
    ret = saveSettings(newSettings);
    wrn("Settings saved, name set to: %s", name);
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
        currentSettings = settings;
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
    dbg("APP_SETTINGS: <<CURRENT APP SETTINGS>>");
    dbg("magic: 0x%08x", getCurrent().magic);
    dbg("version: 0x%08x", getCurrent().version);
    dbg("name: %s", getCurrent().name);
    dbg("wifi0.ssid: %s", getCurrent().wifi0.ssid);
    dbg("wifi0.pass: %s", getCurrent().wifi0.pass);
    dbg("wifi1.ssid: %s", getCurrent().wifi1.ssid);
    dbg("wifi1.pass: %s", getCurrent().wifi1.pass);
    dbg("wifi2.ssid: %s", getCurrent().wifi2.ssid);
    dbg("wifi2.pass: %s", getCurrent().wifi2.pass);
    dbg("APP_SETTINGS: <<CURRENT APP SETTINGS>>\r\n");
}

//------------------------------------------------------------------------------