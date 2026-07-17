#include "bits.h"
using namespace std;

string sticker[1000000][5];

namespace print 
{
    void init_sticker() 
    {
        int o;
        std::ifstream fin("sticker.txt");
        string s;
        while(fin>>s)
        {
            sticker[o][0]=s;
            for(int i=1;i<5;i++)
            {
                fin>>sticker[o][i];
            }
            //fin>>s;
            o++;
        }
        fin.close();
    }
}

int main()
{
    print::init_sticker();
    cout<<sticker[0][0]<<endl;
    cout<<sticker[0][1]<<endl;
    cout<<sticker[0][2]<<endl;
    cout<<sticker[0][3]<<endl;
    cout<<sticker[0][4]<<endl;
    cout<<sticker[1][0]<<endl;
    cout<<sticker[1][1]<<endl;
    cout<<sticker[1][2]<<endl;
    cout<<sticker[1][3]<<endl;
    cout<<sticker[1][4]<<endl;
    return 0;
}