#include "utils.h"
#include "app-settings.h"
#include "dev-server.h"
#include "serial-cmdhandler.h"


//------------------------------------------------------------------------------
static Commander cmdr;
static DevServer server(cmdr);
static SerialCmdHandler serCmdHandler(cmdr);

//------------------------------------------------------------------------------
void setup()
{
    utils::init();
    AppSettings::instance().init();
    server.init();
}

//------------------------------------------------------------------------------
void loop()
{
    server.process();
    serCmdHandler.process();
}

//------------------------------------------------------------------------------
