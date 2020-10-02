#pragma once

#include <cstdint>

#define WIFI_SETTINGS_LEN 20
#define WIFI_SETTINGS_CH_NUM 3

class AppSettings
{

public:

    typedef struct
    {
        char ssid[WIFI_SETTINGS_LEN];
        char pass[WIFI_SETTINGS_LEN];
    } WifiSettings;

    typedef struct
    {
        uint32_t magic;
        uint32_t version;
        WifiSettings wifi0;
        WifiSettings wifi1;
        WifiSettings wifi2;
    } Settings;

public:
    static AppSettings& instance();
    void init();

    const Settings& getDefaults();
    const Settings& getCurrent();

    // Settings options
    bool storeWifiData(uint8_t wifiSettingsNum, AppSettings::WifiSettings& ws);

private:
    AppSettings();
    bool saveSettings(const Settings &settings);
    void readSettings();

    void printCurrentSettings();

private:
    Settings defaultSettings;
    Settings currentSettings;

};