#pragma once

#include <cstdint>
#include <string>

#define WIFI_SETTINGS_LEN 20
#define WIFI_SETTINGS_CH_NUM 3
#define NAME_LEN 20

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
        char name[NAME_LEN];
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
    bool storeName(std::string name);

private:
    AppSettings();
    bool saveSettings(const Settings &settings);
    void readSettings();

    void printCurrentSettings();

private:
    Settings defaultSettings;
    Settings currentSettings;

};