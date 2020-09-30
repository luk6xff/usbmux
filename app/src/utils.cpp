#include "utils.h"
#include "timerManager.h"


#define DBG_BAUDRATE 115200
#define DBG_VERBOSITY_LEVEL DBG_VERBOSE

//------------------------------------------------------------------------------
void utils::init()
{
    Serial.begin(DBG_BAUDRATE);
    Debug.setDebugOutputStream(&Serial);
    Debug.setDebugLevel(DBG_VERBOSITY_LEVEL);
    Debug.timestampOn();
}

//------------------------------------------------------------------------------
void utils::update()
{
    //Update all the timers at once
    TimerManager::instance().update();
}
