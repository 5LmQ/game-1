#include "bits.h"
using namespace std;

namespace tool{
    // 判断一个 Unicode 码点的终端显示宽度（中文/全角=2，其余=1）
    inline int cp_width(uint32_t cp) {
        if (cp < 0x20) return 1;                        // 控制字符
        if ((cp >= 0x1100 && cp <= 0x115F) ||           // 谚文兼容
            (cp >= 0x2E80 && cp <= 0x303E) ||           // CJK 部首/注音
            (cp >= 0x3041 && cp <= 0x33FF) ||           // 假名/CJK 符号
            (cp >= 0x3400 && cp <= 0x4DBF) ||           // CJK 扩展A
            (cp >= 0x4E00 && cp <= 0x9FFF) ||           // CJK 统一汉字（中文）
            (cp >= 0xF900 && cp <= 0xFAFF) ||           // 兼容汉字
            (cp >= 0xFF00 && cp <= 0xFF60) ||           // 全角 ASCII
            (cp >= 0xFFE0 && cp <= 0xFFE6) ||           // 全角符号
            (cp >= 0x20000 && cp <= 0x3FFFD))           // 扩展B+
            return 2;
        return 1;                                       // ASCII、框线 U+2500 等单宽
    }

    // 一个字符串的总显示宽度
    inline int display_width(const std::string& s) {
        int w = 0, p = 0;
        while (p < (int)s.size()) {
            unsigned char c = s[p];
            int clen = (c < 0x80) ? 1 : (c < 0xE0) ? 2 : (c < 0xF0) ? 3 : 4;
            uint32_t cp = 0;
            if (clen == 1)      cp = c;
            else if (clen == 2) cp = ((c & 0x1F) << 6)  | (s[p+1] & 0x3F);
            else if (clen == 3) cp = ((c & 0x0F) << 12) | ((s[p+1] & 0x3F) << 6) | (s[p+2] & 0x3F);
            else                cp = ((c & 0x07) << 18) | ((s[p+1] & 0x3F) << 12) | ((s[p+2] & 0x3F) << 6) | (s[p+3] & 0x3F);
            w += cp_width(cp);
            p += clen;
        }
        return w;
    }

    // 把一行字符串按"每格 cell_cols 显示列"切成若干格子（默认 2 列）
    inline std::vector<std::string> split_cells(const std::string& line, int cell_cols = 2) {
        std::vector<std::string> cells;
        int col = 0;
        std::string cur;
        int i = 0;
        while (i < (int)line.size()) {
            unsigned char c = line[i];
            int clen = (c < 0x80) ? 1 : (c < 0xE0) ? 2 : (c < 0xF0) ? 3 : 4;
            uint32_t cp = 0;
            if (clen == 1)      cp = c;
            else if (clen == 2) cp = ((c & 0x1F) << 6)  | (line[i+1] & 0x3F);
            else if (clen == 3) cp = ((c & 0x0F) << 12) | ((line[i+1] & 0x3F) << 6) | (line[i+2] & 0x3F);
            else                cp = ((c & 0x07) << 18) | ((line[i+1] & 0x3F) << 12) | ((line[i+2] & 0x3F) << 6) | (line[i+3] & 0x3F);
            int w = cp_width(cp);
            if (col + w > cell_cols && !cur.empty()) {   // 当前格放不下 → 另起一格
                cells.push_back(cur);
                cur.clear();
                col = 0;
            }
            cur.append(line.substr(i, clen));            // 整段码点（不会拆坏）
            col += w;
            i += clen;
        }
        if (!cur.empty()) cells.push_back(cur);
        return cells;
    }

    // 统计 UTF-8 字符串的显示字符数（码点数）
    inline int utf8_len(const std::string& s) {
        int n = 0;
        for (unsigned char c : s)
            if ((c & 0xC0) != 0x80) n++;   // 只统计"首字节"，跳过 0x80~0xBF 的后续字节
        return n;
    }

}


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
                    getline(fin,s);
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
                    string cell = screen_char[i][j];
                    int col = screen_color[i][j];
                    if(col != 0) cout<<"\033["<<col<<"m";
                    cout<<cell;
                    if(col != 0) cout<<"\033[0m";
                    // 补齐到 2 显示列，保证网格对齐（中文字/双宽已占满，ASCII 补 1 空格）
                    for(int k=tool::display_width(cell); k<2; k++) cout<<" ";
                }
                cout<<endl;
            }
            cout<<"\033[0m";
        }

        void add_sticker(Sticker s,int x,int y)
        {
            for(int i=0;i<s.height;i++)
            {
                auto cells = tool::split_cells(s.stick[i], 2);   // 每格 2 显示列
                for(int j=0; j<(int)cells.size() && j<s.width; j++)
                {
                    screen_char[i+y][j+x] = cells[j];
                    if(i<(int)s.color.size() && j<(int)s.color[i].size())
                        screen_color[i+y][j+x] = s.color[i][j];
                }
            }
        }
    };
}

/*
容我赘述些许事项
这是一个比较方便的显示库
定义了贴纸、屏幕等类型
init_sticker可以在本地中调取贴纸文件压入sticker_map
贴纸文件地址默认在项目文件夹下叫sticker.txt的文件，地址可以通过更改read_path以调取其他地址、其他命名的贴纸文件
对于贴纸文件的格式可以通过调整terminator来调整其语法标识符
对于screen，resize可以调整屏幕大小，print_screen可以打印屏幕内容
screen中一个像素代表一个方格，一般情况下是2个字符，但不排除中文字符
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