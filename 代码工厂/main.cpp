#include "bits.h"
#include "sticker_tool.h"
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
    // cout<<sticker_map["Empty_square"].height<<endl;
    // cout<<sticker_map["Empty_square"].width<<endl;
    // for(auto s:sticker_map["Empty_square"].stick)
    // {
    //     cout<<s<<endl;
    // }
    screen a;
    a.resize(60,30,"  ",0);
    a.add_sticker(sticker_map["Empty_square"],0,0);
    a.add_sticker(sticker_map["Empty_square"],4,0);

    a.print_screen();
    return 0;
}
