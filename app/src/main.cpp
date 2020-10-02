#include "utils.h"
#include "app-settings.h"
#include "dev-server.h"
#include "serial-cmdhandler.h"



static Commander cmdr;
static DevServer server(cmdr);
static SerialCmdHandler serCmdHandler(cmdr);


void setup()
{
    AppSettings::instance().init();
    utils::init();
    server.init();
}


void loop()
{
    server.process();
    serCmdHandler.process();
    //utils::update();
}
