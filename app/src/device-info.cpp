#pragma once

#include "device-info.h"

DeviceInfo::DeviceInfo()
    : buildAuthor("Build author", "luk6xff")
    , buildDate("Build date", F(__DATE__))
    , buildTime("Build time", F(__TIME__))
    , vcc("VCC", ESP.getVcc(), "V")
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
String DeviceInfo::createDevInfoTable()
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

//------------------------------------------------------------------------------
String DeviceInfo::createDynamicDevInfo()
{
    //String json = "{\"free_heap\":\"" + String(devInfo.freeHeap) + "\",";
    //json += "\"free_stack\":\"" + String(devInfo.freeStack) + "\"}";
    String json = "{" + freeHeap.name() + ":" + freeHeap.val() + "\","
                + freeStack.name() + ":" + freeStack.val()
                + "}";
    return json;
}

//------------------------------------------------------------------------------