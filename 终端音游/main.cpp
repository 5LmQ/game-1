#include "bits.h"

using namespace std;
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define GETCH _getch()
    int get_key_nb() {
        if (_kbhit()) return _getch();
        return -1;
    }
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

        void print_frame(int time)
        {
            cout<<"-------------------------"<<endl;
            for(int i=0;i<10;i++)
            {
                for(int j=1;j<=4;j++)
                {
                    cout<<"|"<<px[frame[time][i][j]];
                }
                cout<<"|"<<endl;
            }
            cout<<"-------------------------         判定线"<<endl;
            cout<<"   F     G     H     J   "<<endl;
            cout<<"按q退出"<<endl;
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
            char a=GETCH;
            cout<<"按任意键继续"<<endl;
            GETCH;
            clear;
        }

        void start()
        {
            cout<<"1。存入谱面"<<endl;
            cout<<"2。删除谱面"<<endl;
            cout<<"3。游玩谱面"<<endl;
            char a=GETCH;
            if(a=='1')
            {
                clear;
                cout<<"输入谱面文件名"<<endl;
                string file_name;
                cin>>file_name;
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
                        cout<<i+1<<"。"<<staff[i+page*5].name<<endl;
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
                        cout<<i+1<<"。"<<staff[i+page*5].name<<endl;
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
            memset(display.frame,0,sizeof(display.frame));
            const char* zhuangtai[4]={" good"," miss"," bad ","     "};
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
            printf("\033[2J\033[H");
            fflush(stdout);

            #ifdef _WIN32
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD written;
            #endif

            auto next_frame = std::chrono::steady_clock::now();

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

                int scan_cnt=min(4,(int)staff_copy.notes.size());
                for(int j=0;j<scan_cnt;j++)
                {
                    int tr=staff_copy.notes[j].track-1;
                    if(note_idx[tr] < 0)
                    {
                        need_tap[tr]=staff_copy.notes[j].etime-i;
                        note_idx[tr]=j;
                    }
                }

                vector<int> to_erase;

                for(int j=0;j<4;j++)
                {
                    if(note_idx[j]<0) continue;

                    int dt=need_tap[j];

                    if(dt<-2*2)
                    {
                        zt[j]=1;
                        miss++;
                        to_erase.push_back(note_idx[j]);
                        continue;
                    }

                    bool key_pressed=false;
                    for(int k=0;k<4;k++)
                    {
                        if(anjian[k]==push[j]) { key_pressed=true; break; }
                    }

                    if(!key_pressed) continue;

                    if(dt>=-2*2 && dt<=2*2)
                    {
                        zt[j]=0;
                        good++;
                        to_erase.push_back(note_idx[j]);
                    }
                    else if(dt>2*2 && dt<=6*2)
                    {
                        zt[j]=2;
                        bad++;
                        for(int k=i;k<staff_copy.notes[j].etime;k++)
                        {
                            int time = i-staff_copy.notes[j].stime;
                            int s=staff_copy.notes[j].get_speed()*time;
                            display.frame[i][s][staff_copy.notes[j].track]=1;
                        }
                        to_erase.push_back(note_idx[j]);
                    }
                }

                sort(to_erase.begin(),to_erase.end(),greater<int>());
                for(int idx:to_erase)
                {
                    staff_copy.notes.erase(staff_copy.notes.begin()+idx);
                }

                char buf[4096];
                int len = 0;

                len += sprintf(buf+len, "\033[H");
                len += sprintf(buf+len, "%d %d %d %d\033[K\n",
                    need_tap[0], need_tap[1], need_tap[2], need_tap[3]);
                len += sprintf(buf+len, "%d %d %d %d\033[K\n",
                    note_idx[0], note_idx[1], note_idx[2], note_idx[3]);
                len += sprintf(buf+len, "good:%d miss:%d bad:%d\033[K\n", good, miss, bad);

                for(int j=0;j<4;j++)
                    len += sprintf(buf+len, "|%s", zhuangtai[zt[j]]);
                len += sprintf(buf+len, "|         \xe7\x8a\xb6\xe6\x80\x81\xe6\xa0\x8f\033[K\n");

                len += sprintf(buf+len, "-------------------------\033[K\n");
                for(int row=0;row<10;row++)
                {
                    for(int col=1;col<=4;col++)
                        len += sprintf(buf+len, "|%s", px_cstr[display.frame[i][row][col]]);
                    len += sprintf(buf+len, "|\033[K\n");
                }
                len += sprintf(buf+len, "-------------------------         \xe5\x88\xa4\xe5\xae\x9a\xe7\xba\xbf\033[K\n");
                len += sprintf(buf+len, "   F     G     H     J   \033[K\n");
                len += sprintf(buf+len, "\xe6\x8c\x89q\xe9\x80\x80\xe5\x87\xba\033[K\n");
                len += sprintf(buf+len, "\033[J");

                #ifdef _WIN32
                WriteConsoleA(hOut, buf, (DWORD)len, &written, NULL);
                #else
                ::write(STDOUT_FILENO, buf, len);
                #endif

                next_frame += std::chrono::milliseconds(8);
                std::this_thread::sleep_until(next_frame);
            }

            show_cursor();
            printf("\033[2J\033[H");
            fflush(stdout);
            cout<<"游玩结束"<<endl;
            cout<<"谱面："<<staff[staff_num].name<<endl;
            cout<<"谱面长度："<<staff[staff_num].time<<"Z"<<endl;
            cout<<"谱面物量"<<staff[staff_num].notes.size()<<endl;
            cout<<"成绩"<<endl<<"good:"<<good<<" miss:"<<miss<<" bad:"<<bad<<endl;
            cout<<"按下任意键继续"<<endl;
            GETCH;
        }
        
}game;

int main()
{

    #ifdef _WIN32
        SetConsoleOutputCP(65001);  // Windows GBK
        SetConsoleCP(65001);
    #else
        setlocale(LC_ALL, "");    // Linux/Mac UTF-8
    #endif

    Staff test;
    test.name="test";
    test.time=240;
    test.notes={{0,20,1},{20,40,2},{40,60,3},{60,80,4},
                {100,140,1},{120,140,2},{140,180,3},{180,200,4}};
    game.staff.push_back(test);   // 用 push_back，不要 staff[0]

    while(true)
    {
        game.start();
    }
    return 0;
}