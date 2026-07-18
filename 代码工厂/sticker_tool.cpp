#include "bits.h"
using namespace std;


namespace sticker_tool{

    string read_path="sticker.txt";
    string terminator="end";
    string name_terminator="name:";
    string stick_terminator="stick:";
    string width_terminator="width:";
    string height_terminator="height:";
    string color_terminator="color:";

    struct Sticker
    {
        int height;
        int width;
        vector<string> stick;
        vector<vector<int>> color;
    };

    map<string, Sticker> sticker_map;
    void init_sticker() 
    {
        std::ifstream fin(read_path);
        if(!fin)
        {
            cerr<<"贴纸文件打开失败: "<<read_path<<endl;
            return;
        }
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
                while(getline(fin,s) && s!=terminator && s!=color_terminator)
                {
                    sticker_tmp.stick.push_back(s);
                }
                if(s==color_terminator)
                {
                    for(int r=0;r<sticker_tmp.height;r++)
                    {
                        if(!getline(fin,s)) break;       
                        if(s==terminator) break;         
                        istringstream iss(s);
                        vector<int> row;
                        int v;
                        while(iss>>v) row.push_back(v);
                        sticker_tmp.color.push_back(row);
                    }
                }
                if(s==terminator)
                {
                    sticker_map[name]=sticker_tmp;
                    sticker_tmp = Sticker(); 
                }
            }
        }
    }

    struct screen
    {
        int screen_width;
        int screen_height;
        std::vector<std::vector<string>> screen_char;
        std::vector<std::vector<int>> screen_color;
        screen(int w=0,int h=0):screen_width(w),screen_height(h),screen_char(h, std::vector<string>(w," ")),screen_color(h, std::vector<int>(w,0)){}

        void resize(int w,int h,string fill=" ",int color=0) {
            screen_width = w;
            screen_height = h;
            screen_char.assign(h, std::vector<string>(w, fill));
            screen_color.assign(h, std::vector<int>(w, color));
        }

        void print_screen()
        {
            for(int i=0;i<screen_height;i++)
            {
                for(int j=0;j<screen_width;j++)
                {
                    int col = screen_color[i][j];
                    if(col != 0) 
                    {
                        cout<<"\033["<<col<<"m";
                    }
                    cout<<screen_char[i][j];
                    if(col != 0)
                    {
                        cout<<"\033[0m";
                    }
                }
                cout<<endl;
            }
            cout<<"\033[0m";
        }

        void add_sticker(Sticker s,int x,int y)
        {
            for(int i=0;i<s.height;i++)
            {
                for(int j=0;j<s.width;j++)
                {
                    screen_char[i+y][j+x]=s.stick[i][j];
                }
            }
        }
    };
}

/*
容我再次赘述些许事项
这是一个比较方便的显示库
定义了贴纸、屏幕等类型
init_sticker可以在本地中调取贴纸文件压入sticker_map
贴纸文件地址默认在项目文件夹下叫sticker.txt的文件，地址可以通过更改read_path以调取其他地址、其他命名的贴纸文件
对于贴纸文件的格式可以通过调整terminator来调整其语法标识符
对于screen，resize可以调整屏幕大小，print_screen可以打印屏幕内容
add_sticker可以将贴纸添加到屏幕上，x、y为贴纸左上角坐标
理论上控制移动后贴纸的位移可以做到悬浮ui和弹窗
再者如果调整加载顺序，可以做到图层
*/

/*
对于颜色编码
0 默认颜色
1 高亮
4 下划线
5 闪烁
7 反显
30 黑色
31 红色
32 绿色
33 黄色
34 蓝色
35 紫色
36 青色
37 白色
40 背景黑色
41 背景红色
42 背景绿色
43 背景黄色
44 背景蓝色
45 背景紫色
46 背景青色
47 背景白色
90 灰色
91 亮红色
92 亮绿色
93 亮黄色
94 亮蓝色
95 亮紫色
96 亮青色
97 白色
100 深灰色
101 亮红色
102 亮绿色
103 亮黄色
104 亮蓝色
105 亮紫色
106 亮青色
107 白色


*/