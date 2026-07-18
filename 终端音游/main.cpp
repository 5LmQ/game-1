#include "bits.h"
#include <csignal>

using namespace std;

void signal_handler(int sig) {
    printf("\033[?25h");
    fflush(stdout);
    exit(0);
}


#define endl "\n"

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    BOOL WINAPI ctrl_handler(DWORD dwCtrlType) {
        if (dwCtrlType == CTRL_C_EVENT || dwCtrlType == CTRL_BREAK_EVENT) {
            printf("\033[?25h");
            fflush(stdout);
            exit(0);
        }
        return FALSE;
    }
    #define GETCH _getch()
    int get_key_nb() {
        if (_kbhit()) return _getch();
        return -1;
    }
    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif
    void enable_vt100() {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        GetConsoleMode(hOut, &mode);
        SetConsoleMode(hOut, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    void hide_cursor() { printf("\033[?25l"); }
    void show_cursor() { printf("\033[?25h"); }
    #define clear system("cls")
    #define move_home() printf("\033[H")
#else
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    int get_key_nb() {
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

        int ch = getchar();
        if (ch == EOF) ch = -1;

        fcntl(STDIN_FILENO, F_SETFL, flags);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
    void enable_vt100() {}
    void hide_cursor() { printf("\033[?25l"); }
    void show_cursor() { printf("\033[?25h"); }
    #define clear system("clear")
    #define move_home() printf("\033[H")
    char getch_char() {
        struct termios oldt, newt;
        char ch;
        tcgetattr(STDIN_FILENO, &oldt);
        newt=oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch=getchar();
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
    #define GETCH getch_char()
#endif

char push[5]={'f','g','h','j'};

//通常来说单位速度为一tick下降一格

struct Note
{
    int stime;//开始时间
    int etime;//结束时间
    int track;//轨道
    double get_speed()
    {
        int dt=etime-stime;
        double ds=10.0;
        return ds/dt;
    }
};

struct Staff
{
    string name;
    int time;
    vector<Note> notes;
};
class Dispaly
{
    public:
        bool frame[1000000][10][5];  // track 1-4, index 0 unused
        string px[2]={"     ","-----"};
        
        void add_note_to_frame(Note n)
        {
            for(int i=n.stime;i<n.etime;i++)
            {
                int time = i-n.stime;
                int s=n.get_speed()*time;

                frame[i][s][n.track]=1;
            }
        }

        string get_frame_str(int time)
        {
            string s;
            s.reserve(512);
            s += "-------------------------\n";
            for(int i=0;i<10;i++)
            {
                for(int j=1;j<=4;j++)
                {
                    s += "|";
                    s += px[frame[time][i][j]];
                }
                s += "|\n";
            }
            s += "-------------------------         判定线\n";
            s += "   F     G     H     J   \n";
            s += "按q退出\n";
            return s;
        }
}display;


class Game
{
    public:
        vector<Staff> staff;

        void upload_staff(string file_path)
        {
            ifstream fin(file_path);
            if(!fin)
            {
                cout<<"谱面文件打开失败"<<endl;
                cout<<"按任意键返回"<<endl;
                GETCH;
                clear;
                return;
            }
            Staff staff_tmp;
            getline(fin,staff_tmp.name);
            fin>>staff_tmp.time;
            int stime,etime,track;
            while(fin>>stime>>etime>>track)
            {
                staff_tmp.notes.push_back({stime,etime,track});
            }

            fin.close();
            staff.push_back(staff_tmp);

            cout<<"谱面上传成功"<<endl;
            cout<<"谱面名称："<<staff_tmp.name<<endl;
            cout<<"谱面时间："<<staff_tmp.time<<endl;
            cout<<"谱面音符数量："<<staff_tmp.notes.size()<<endl;
            cout<<"按任意键继续"<<endl;
            GETCH;
            clear;
        }

        void start()
        {
            cout<<"1.存入谱面"<<endl;
            cout<<"2.删除谱面"<<endl;
            cout<<"3.游玩谱面"<<endl;
            cout<<"4.退出游戏"<<endl;
            char a=GETCH;
            if(a=='4' || a==3)
            {
                show_cursor();
                exit(0);
            }
            if(a=='1')
            {
                clear;
                cout<<"输入谱面文件名（不含.txt）"<<endl;
                string file_name;
                getline(cin, file_name);
                if(file_name.empty())
                {
                    clear;
                    return;
                }
                string file_path="staff/"+file_name+".txt";
                upload_staff(file_path);
                clear;
            }
            else if(a=='2')
            {
                clear;
                int page=0;
                int maxpage=staff.size()/5+1;
                while(1)
                {

                    for(int i=0;i<5;i++)
                    {
                        if(i+page*5>=staff.size())
                        {
                            cout<<i+1<<".无"<<endl;
                        }
                        else
                        {
                            cout<<i+1<<"."<<staff[i+page*5].name<<endl;
                        }
                    }

                    cout<<"当前页数："<<page+1<<"/"<<maxpage<<endl;
                    cout<<"z上一页.  ";
                    cout<<"x下一页.  ";
                    cout<<"q返回.   "<<endl;
                    cout<<"1～5删除谱面"<<endl;
                    char a=GETCH;
                    if(a>='1'&&a<='5')
                    {
                        if(staff.size()>=a-'0'+page*5)
                        {
                            cout<<"是否删除谱面"<<staff[a-'0'-1+page*5].name<<"？y/n"<<endl;
                            char b=GETCH;
                            if(b=='y')
                            {
                                staff[a-'0'-1+page*5].name="";
                                staff.erase(staff.begin()+(a-'0'-1+page*5));
                                maxpage=staff.size()/5+1;
                                page=0;
                                cout<<"删除成功"<<endl;
                                cout<<"按任意键继续"<<endl;
                                GETCH;
                                clear;
                            }
                        }
                    }
                    else if(a=='z')
                    {
                        if(page>0)
                        {
                            page--;
                        }
                    }
                    else if(a=='x')
                    {
                        if(page<maxpage-1)
                        {
                            page++;
                        }
                    }
                    else if(a=='q')
                    {
                        clear;
                        break;
                    }
                    else
                    {
                        clear;
                        continue;
                    }
                    clear;
                }
            }
             else if(a=='3')
            {
                clear;
                int page=0;
                int maxpage=staff.size()/5+1;
                while(1)
                {

                    for(int i=0;i<5;i++)
                    {
                        if(i+page*5>=staff.size())
                        {
                            cout<<i+1<<".无"<<endl;
                        }
                        else
                        {
                            cout<<i+1<<"."<<staff[i+page*5].name<<endl;
                        }
                    }

                    cout<<"当前页数："<<page+1<<"/"<<maxpage<<endl;
                    cout<<"z上一页.  ";
                    cout<<"x下一页.  ";
                    cout<<"q返回.   "<<endl;
                    cout<<"1～5游玩谱面"<<endl;
                    char a=GETCH;
                    if(a>='1'&&a<='5')
                    {
                        if(staff.size()>=a-'0'+page*5)
                        {
                            cout<<"开始游玩"<<staff[a-'0'-1+page*5].name<<endl;
                            start_play(a-'0'-1+page*5);
                        }
                    }
                    else if(a=='z')
                    {
                        if(page>0)
                        {
                            page--;
                        }
                    }
                    else if(a=='x')
                    {
                        if(page<maxpage-1)
                        {
                            page++;
                        }
                    }
                    else if(a=='q')
                    {
                        clear;
                        break;
                    }
                    else
                    {
                        clear;
                        continue;
                    }
                    clear;
                }
            }
            else
            {
                clear;
            }
        }

        void start_play(int staff_num)
        {
            clear;
            memset(display.frame,0,sizeof(display.frame));
            const char* zhuangtai[4]={"GOOD ","MISS ","BAD  ","     "};
            const char* px_cstr[2]={"     ","-----"};
            int good=0,miss=0,bad=0;
            Staff staff_copy=staff[staff_num];
            sort(staff_copy.notes.begin(),staff_copy.notes.end(),[](Note a,Note b){return a.etime<b.etime;});
            for(auto n:staff_copy.notes)
            {
                display.add_note_to_frame(n);
            }

            enable_vt100();
            hide_cursor();
            // 切换到备用屏幕缓冲区，避免污染主屏幕和滚动历史
            printf("\033[?1049h\033[2J\033[H");
            fflush(stdout);

            #ifdef _WIN32
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD written;
            #endif

            auto next_frame = std::chrono::steady_clock::now();
            int total_notes = staff_copy.notes.size();

            for(int i=0;i<staff_copy.time;i++)
            {
                char anjian[4];
                for(int k=0;k<4;k++) anjian[k]=get_key_nb();
                if(anjian[0]=='q'||anjian[1]=='q'||anjian[2]=='q'||anjian[3]=='q')
                {
                    break;
                }

                int need_tap[4]={0,0,0,0};
                int note_idx[4]={-1,-1,-1,-1};
                int zt[4]={3,3,3,3};

                // 找每个轨道最近的音符
                for(int j=0;j<(int)staff_copy.notes.size();j++)
                {
                    int tr=staff_copy.notes[j].track-1;
                    if(tr<0||tr>3) continue;
                    if(note_idx[tr] < 0)
                    {
                        need_tap[tr]=staff_copy.notes[j].etime-i;
                        note_idx[tr]=j;
                    }
                }

                vector<int> to_erase;

                const int GOOD_WINDOW = 10;
                const int BAD_WINDOW = 22;
                const int MISS_WINDOW = 30;

                for(int j=0;j<4;j++)
                {
                    if(note_idx[j]<0) continue;

                    int dt=need_tap[j];

                    bool key_pressed=false;
                    for(int k=0;k<4;k++)
                    {
                        if(anjian[k]==push[j]) { key_pressed=true; break; }
                    }

                    if(key_pressed)
                    {
                        int abs_dt = dt < 0 ? -dt : dt;
                        if(abs_dt <= GOOD_WINDOW)
                        {
                            zt[j]=0;
                            good++;
                            to_erase.push_back(note_idx[j]);
                        }
                        else if(abs_dt <= BAD_WINDOW)
                        {
                            zt[j]=2;
                            bad++;
                            to_erase.push_back(note_idx[j]);
                        }
                    }
                    else if(dt < -MISS_WINDOW)
                    {
                        zt[j]=1;
                        miss++;
                        to_erase.push_back(note_idx[j]);
                    }
                }

                sort(to_erase.begin(),to_erase.end(),greater<int>());
                for(int idx:to_erase)
                {
                    staff_copy.notes.erase(staff_copy.notes.begin()+idx);
                }

                string frame_str = display.get_frame_str(i);

                // 预估缓冲区大小：头部信息 + frame_str + ANSI 转义序列
                size_t buf_size = 512 + frame_str.size();
                vector<char> buf(buf_size);
                int len = 0;

                len += snprintf(buf.data() + len, buf_size - len, "\033[H");
                // 谱面信息栏
                len += snprintf(buf.data() + len, buf_size - len, " %-20s  %d/%d frames\033[K\n",
                    staff[staff_num].name.c_str(), i, staff_copy.time);
                len += snprintf(buf.data() + len, buf_size - len, " GOOD:%-4d  BAD:%-4d  MISS:%-4d\033[K\n", good, bad, miss);
                len += snprintf(buf.data() + len, buf_size - len, "\033[K\n");

                // 状态判定显示
                len += snprintf(buf.data() + len, buf_size - len, " ");
                for(int j=0;j<4;j++)
                    len += snprintf(buf.data() + len, buf_size - len, " %s ", zhuangtai[zt[j]]);
                len += snprintf(buf.data() + len, buf_size - len, "\033[K\n");

                // 游戏区域（使用 get_frame_str 渲染）
                len += snprintf(buf.data() + len, buf_size - len, "%s", frame_str.c_str());

                // 收尾清屏
                len += snprintf(buf.data() + len, buf_size - len, "\033[J");

                #ifdef _WIN32
                WriteConsoleA(hOut, buf.data(), (DWORD)len, &written, NULL);
                #else
                ::write(STDOUT_FILENO, buf.data(), len);
                #endif

                next_frame += std::chrono::milliseconds(8);
                std::this_thread::sleep_until(next_frame);
            }

            // 切回主屏幕缓冲区
            printf("\033[?1049l");
            show_cursor();
            fflush(stdout);
            clear;
            cout<<endl;
            cout<<"========== 游玩结束 =========="<<endl;
            cout<<" 谱面：  "<<staff[staff_num].name<<endl;
            cout<<" 长度：  "<<staff[staff_num].time<<" 帧 ("
                <<staff[staff_num].time*8<<"ms)"<<endl;
            cout<<" 物量：  "<<total_notes<<endl;
            cout<<" 成绩：  GOOD:"<<good<<"  BAD:"<<bad<<"  MISS:"<<miss<<endl;
            if(total_notes>0)
            {
                int score = good*100/(total_notes);
                cout<<" 得分：  "<<score<<"/100"<<endl;
            }
            cout<<"=============================="<<endl;
            cout<<endl<<"按任意键继续..."<<endl;
            GETCH;
        }
        
}game;

int main()
{
    ios::sync_with_stdio(false);
    cin.tie(0);
    cout.tie(0);
    signal(SIGINT, signal_handler);
    #ifdef _WIN32
        SetConsoleOutputCP(65001);  // Windows GBK
        SetConsoleCP(65001);
        SetConsoleCtrlHandler(ctrl_handler, TRUE);
    #else
        setlocale(LC_ALL, "");    // Linux/Mac UTF-8
    #endif

    while(true)
    {
        game.start();
    }
    return 0;
}