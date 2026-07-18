#include "bits.h"
using namespace std;

namespace sticker_tool{

    string read_path="sticker.txt";
    string terminator="end";
    string name_terminator="name:";
    string stick_terminator="stick:";
    string width_terminator="width:";
    string height_terminator="height:";

    struct Sticker
    {
        int height;
        int width;
        vector<string> stick;
    };

    map<string, Sticker> sticker_map;
    void init_sticker() 
    {
        std::ifstream fin(read_path);
        string s;
        string name;
        Sticker sticker_tmp;
        while(getline(fin,s))
        {
            if(s==name_terminator)
            {
                getline(fin,name);
            }
            else if(s==width_terminator)
            {
                getline(fin,s);
                sticker_tmp.width=stoi(s);
            }
            else if(s==height_terminator)
            {
                getline(fin,s);
                sticker_tmp.height=stoi(s);
            }
            else if(s==stick_terminator)
            {
                while(getline(fin,s))
                {
                    if(s==terminator)
                    {
                        sticker_map[name]=sticker_tmp;
                        sticker_tmp.height=0;
                        sticker_tmp.width=0;
                        sticker_tmp.stick.clear();
                        break;
                    }        
                    sticker_tmp.stick.push_back(s);
                }
            }
        }
    }
}

int main()
{
    sticker_tool::init_sticker();
    cout<<sticker_tool::sticker_map["Empty_square"].height<<endl;
    cout<<sticker_tool::sticker_map["Empty_square"].width<<endl;
    for(auto s:sticker_tool::sticker_map["Empty_square"].stick)
    {
        cout<<s<<endl;
    }


    return 0;
}
