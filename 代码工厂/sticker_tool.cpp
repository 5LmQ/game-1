#include "bits.h"
using namespace std;

namespace display{

    string read_path="sticker.txt";
    string terminator="end";


    struct Sticker
    {
        int height;
        int width;
        vector<string> stick;

        map<string, Sticker> sticker_map;
        void init_sticker() 
        {
            std::ifstream fin(read_path);
            Sticker sticker_tmp;
            string name;
            getline(fin,name);
            string s;
            while(getline(fin,s))
            {

                if(s=="end")
                {
                    sticker_map[name]=sticker_tmp;
                    sticker_tmp.height=0;
                    sticker_tmp.width=0;
                    sticker_tmp.stick.clear();
                    getline(fin,name);
                }
                sticker_tmp.stick.push_back(s);
                sticker_tmp.height++;
                sticker_tmp.width=max(sticker_tmp.width,int(s.length()/2));
            }
        }
    };
}


int main()
{
    display::Sticker sticker;
    sticker.init_sticker();
    cout<<sticker.sticker_map["1"].height<<endl;
    cout<<sticker.sticker_map["1"].width<<endl;
    for(auto s:sticker.sticker_map["1"].stick)
    {
        cout<<s<<endl;
    }
    return 0;
}