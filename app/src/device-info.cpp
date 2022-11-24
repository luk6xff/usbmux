#include "device-info.h"
#include "app-settings.h"

DeviceInfo::DeviceInfo()
    : name("Name", AppSettings::instance().getCurrent().name)
    , buildAuthor("Build author", "luk6xff")
    , buildDate("Build date", F(__DATE__))
    , buildTime("Build time", F(__TIME__))
    , buildVersion("Build version", AppSettings::instance().getCurrent().version)
    , vcc("VCC", ESP.getVcc()*3300/0xFFFF, "mV")
    , chipId("Chip ID", ESP.getChipId())
    , fullVersion("Libs version", ESP.getFullVersion())
    , bootVersion("Boot version", ESP.getBootVersion())
    , cpuFreqMhz("CPU Freq", ESP.getCpuFreqMHz(), "MHz")
    , resetInfo("Last Reset info", ESP.getResetInfo())
    , flashChipRealSize("Chip ID", ESP.getFlashChipSize())
    , appSize("Application size", ESP.getSketchSize(), "Bytes")
    , freeHeap("Free Heap", ESP.getFreeHeap(), "Bytes", "glyphicon-indent-left")
    , freeStack("Free Stack", ESP.getFreeContStack(), "Bytes", "glyphicon-tint")
{

}

//------------------------------------------------------------------------------
String DeviceInfo::latest()
{
    String res = "\033[1;35m >>>>>>>>>>>>>>>>>>>> DeviceInfo <<<<<<<<<<<<<<<<<<<< \033[1;39m\r\n";
    res += name.toValueString() + "\r\n"
            + buildAuthor.toValueString() + "\r\n"
            + buildDate.toValueString() + "\r\n"
            + buildTime.toValueString() + "\r\n"
            + buildVersion.toValueString() + "\r\n"
            + vcc.toValueString() + "\r\n"
            + chipId.toValueString() + "\r\n"
            + fullVersion.toValueString() + "\r\n"
            + bootVersion.toValueString() + "\r\n"
            + cpuFreqMhz.toValueString() + "\r\n"
            + resetInfo.toValueString() + "\r\n"
            + flashChipRealSize.toValueString() + "\r\n"
            + appSize.toValueString() + "\r\n"
            + freeHeap.toValueString() + "\r\n"
            + freeStack.toValueString();
    res += "\r\n";
    return res;
}

//------------------------------------------------------------------------------
String DeviceInfo::createDevInfoTable()
{
    String json = "[";
    json += name.toServerJson() + ","
            + buildAuthor.toServerJson() + ","
            + buildDate.toServerJson() + ","
            + buildTime.toServerJson() + ","
            + buildVersion.toServerJson() + ","
            + vcc.toServerJson() + ","
            + chipId.toServerJson() + ","
            + fullVersion.toServerJson() + ","
            + bootVersion.toServerJson() + ","
            + cpuFreqMhz.toServerJson() + ","
            + resetInfo.toServerJson() + ","
            + flashChipRealSize.toServerJson() + ","
            + appSize.toServerJson() + ","
            + freeHeap.toServerJson() + ","
            + freeStack.toServerJson();
    json += "]";
    return json;
}

//------------------------------------------------------------------------------
String DeviceInfo::createDynamicDevInfo()
{
    String json = "{" + freeHeap.name() + ":" + freeHeap.val() + "\","
                + freeStack.name() + ":" + freeStack.val()
                + "}";
    return json;
}

//------------------------------------------------------------------------------