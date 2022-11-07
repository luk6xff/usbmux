#include <deque>

class buforCC
{
public:
    std::deque<char*> BuforMemory;
    int BuforIndex = 0;
    void AddMemory(char* msg);
    char* ScrollMemory();
};