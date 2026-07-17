#include "bits.h"
using namespace std;

namespace print 
{
    map<string, array<string,5>> sticker;
    void init_sticker() 
    {
        std::ifstream fin("sticker.txt");
        string s,name;
        array<string,5> stick;
        while(fin>>s)
        {
            name=s;
            for(int i=0;i<5;i++)
            {
                fin>>stick[i];
            }
            sticker[name]=stick;
        }
        fin.close();
    }
}