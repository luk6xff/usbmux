#include "bufor.h"
#include <deque>
#include <string.h>

void buforCC::AddMemory(char* msg) {
    char* msg_cp;
    strcpy(msg_cp, msg);
    buforCC::BuforMemory.push_back(msg);
}

char* buforCC::ScrollMemory() {
    buforCC::BuforIndex ++;
    if (buforCC::BuforIndex == buforCC::BuforMemory.size()) {
        buforCC::BuforIndex = 0;
    }
    return buforCC::BuforMemory.at(buforCC::BuforIndex);
    //return buforCC::BuforMemory.at(0);
} 

