#include "utils.h"

#include "dev-server.h"
#include "serial-cmdhandler.h"






//static DevServer server;
static Commander cmdr;
static SerialCmdHandler serCmdHandler(cmdr);


void setup()
{
    DBG.begin(DBG_BAUDRATE);
}


void loop()
{

    //server.process();
    serCmdHandler.process();
    static int i = 0;
    if ((i++ % 10000000) == 0)
        Serial.println("TEST");
}



