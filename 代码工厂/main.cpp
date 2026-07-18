#include "bits.h"
#include "sticker_tool.cpp"
#ifdef _WIN32
#define clear system("cls")
#else
#define clear system("clear")
#endif

using namespace std;
using namespace sticker_tool;

int main() 
{
    clear;
    init_sticker();
    screen a;
    a.resize(60,30,"  ",7);
    a.print_screen();
    return 0;
}
