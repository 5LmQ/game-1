#include "bits.h"
//本文件提供一种方法，可以实现在终端实行可视化书写谱面
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
    #include <csignal>
    volatile sig_atomic_t winch_flag = 0;
    void handle_winch(int) { winch_flag = 1; }
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
using namespace std;

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

int time_excel[1000000][4];
string px[7]={"  |  ","  :  "," [s] "," [e] "};

void preview_staff(int time_start,int time_end)
{
    for(int i=time_start;i<=time_end;i+=1)
    {
        for(int j=0;j<4;j++)
        {
            cout<<" "<<px[time_excel[i][j]]<<" ";
        }
        cout<<i;
        cout<<endl;
    }
    fflush(stdout);
}

void add_note_to_time_excel(Note n)
{
    time_excel[n.stime][n.track-1]=2;
    time_excel[n.etime][n.track-1]=3;
    for(int i=n.stime+1;i<n.etime;i++)
    {
        time_excel[i][n.track-1]=1;
    }
}

void start_ide()
{
    map<pair<int,int>,Note> notes_map;
    signal(SIGWINCH, handle_winch);
    int time_window_s=0;
    int time_window_e=30;
    while (1)
    {
        memset(time_excel,0,sizeof(time_excel));
        for (auto& kv : notes_map) {
            auto key  = kv.first;    // 即 pair<etime, track>
            Note& note = kv.second;  // 即 Note 对象
            add_note_to_time_excel(note);
        }
        clear;
        preview_staff(time_window_s,time_window_e);
        cout<<"按w向前移动时间窗口     按s向后移动时间窗口"<<endl;
        cout<<"按c添加一个note   按d删除一个note   按e编辑一个note"<<endl;
        cout<<"按q保存谱面并退出"<<endl;
        if(winch_flag)
        {
            winch_flag=0;
            continue;
        }
        int ch=GETCH;
        if(ch=='w')
        {
            if(time_window_s!=0)
            {
                time_window_s--;
                time_window_e--;
            }
        }
        if(ch=='s')
        {
            if(time_window_e!=1000000)
            {
                time_window_s++;
                time_window_e++;
            }
        }
        if(ch=='c')
        {
            Note n;
            clear;
            preview_staff(time_window_s,time_window_e);
            cout<<"键入note轨道"<<endl;
            char a=GETCH;
            n.track=a-'0';
            clear;
            preview_staff(time_window_s,time_window_e);
            cout<<"输入note开始时间与结束时间"<<endl;
            cin>>n.stime>>n.etime;
            notes_map[{n.etime,n.track}]=n;

        }
        if(ch=='d')
        {
            clear;
            preview_staff(time_window_s,time_window_e);
            int etime,track;
            cout<<"键入note轨道"<<endl;
            char a=GETCH;
            track=a-'0';
            clear;
            preview_staff(time_window_s,time_window_e);
            cout<<"输入note结束时间"<<endl;
            cin>>etime;
            notes_map.erase({etime,track});
        }
        if(ch=='e')
        {
                clear;
                preview_staff(time_window_s,time_window_e);
                int etime,track;
                cout<<"键入note轨道"<<endl;
                char a=GETCH;
                track=a-'0';
                clear;
                preview_staff(time_window_s,time_window_e);
                cout<<"输入note结束时间"<<endl;
                cin>>etime;
                while(1)
                {
                    clear;
                            memset(time_excel,0,sizeof(time_excel));
                            for (auto& kv : notes_map) {
                                auto key  = kv.first;    // 即 pair<etime, track>
                                Note& note = kv.second;  // 即 Note 对象
                                add_note_to_time_excel(note);
                            }
                    preview_staff(time_window_s,time_window_e);
                    cout<<"按s编辑起始时间  按e编辑结束时间  按a向左移动轨道  按d向右移动轨道"<<endl<<"按q退出"<<endl;
                    int ch=GETCH;
                    if(ch=='q')
                    {
                        break;
                    }
                    if(ch=='s')
                    {
                        clear;
                        while(1)
                        {
                            clear;
                            memset(time_excel,0,sizeof(time_excel));
                            for (auto& kv : notes_map) {
                                auto key  = kv.first;    // 即 pair<etime, track>
                                Note& note = kv.second;  // 即 Note 对象
                                add_note_to_time_excel(note);
                            }
                            preview_staff(time_window_s,time_window_e);
                            cout<<"按w向前移动开始时间  按s向后移动开始时间"<<endl;
                            cout<<"按q退出"<<endl;
                            int ch=GETCH;
                            if(ch=='w')
                            {
                                if(notes_map[{etime,track}].stime!=0)
                                {
                                    notes_map[{etime,track}].stime--;
                                }
                            }
                            if(ch=='s')
                            {
                                if(notes_map[{etime,track}].stime<notes_map[{etime,track}].etime-1)
                                {
                                    notes_map[{etime,track}].stime++;
                                }
                            }
                            if(ch=='q')
                            {
                                break;
                            }
                        }
                    }
                    if(ch=='e')
                    {
                        clear;
                        while(1)
                        {
                            clear;
                            memset(time_excel,0,sizeof(time_excel));
                            for (auto& kv : notes_map) {
                                auto key  = kv.first;    // 即 pair<etime, track>
                                Note& note = kv.second;  // 即 Note 对象
                                add_note_to_time_excel(note);
                            }
                            preview_staff(time_window_s,time_window_e);
                            cout<<"按w向前移动结束时间  按s向后移动结束时间"<<endl;
                            cout<<"按q退出"<<endl;
                            int ch=GETCH;
                            if(ch=='w')
                            {
                                Note n = notes_map[{etime,track}];
                                if(n.etime>n.stime+1)
                                {
                                    n.etime--;
                                    notes_map.erase({etime,track});
                                    notes_map[{n.etime,n.track}] = n;
                                    etime = n.etime;
                                }
                            }
                            if(ch=='s')
                            {
                                Note n = notes_map[{etime,track}];
                                if(n.etime<1000000)
                                {
                                    n.etime++;
                                    notes_map.erase({etime,track});
                                    notes_map[{n.etime,n.track}] = n;
                                    etime = n.etime;
                                }
                            }
                            if(ch=='q')
                            {
                                break;
                            }
                        }
                    }
                    if(ch=='a')
                    {
                        Note n = notes_map[{etime,track}];
                        if(n.track > 1)
                        {
                            n.track--;
                            notes_map.erase({etime,track});
                            notes_map[{n.etime, n.track}] = n;
                            track = n.track;
                        }
                    }
                    if(ch=='d')
                    {
                        Note n = notes_map[{etime,track}];
                        if(n.track < 4)
                        {
                            n.track++;
                            notes_map.erase({etime,track});
                            notes_map[{n.etime, n.track}] = n;
                            track = n.track;
                        }
                    }
                }
                if(ch=='q')
                {
                    break;
                }
        }
        if(ch=='q')
        {
            clear;
            cout<<"正在导出谱面"<<endl;
            cout<<"导出成功，请输入谱面名称"<<endl;
            string name;
            getline(cin,name);
            ofstream fout("staff/"+name+".txt");
            fout<<name<<endl;
            cout<<"请输入谱面时长（单位：帧）"<<endl;
            int time_length;
            cin>>time_length;
            fout<<time_length<<endl;
            for (auto& kv : notes_map) 
            {
                auto key  = kv.first;    
                Note& note = kv.second;  
                fout<<note.stime<<" "<<note.etime<<" "<<note.track<<endl;
            }
            cout<<"导出成功"<<endl;
            cout<<"按任意键继续"<<endl;
            GETCH;
            break;
        }
    
    }
}