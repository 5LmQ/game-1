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
string px[7]={"  |  ","  :  "," [s] "," [e] ","| : |","|[s]|","|[e]|"};

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
    Staff staff;
    int time_window_s=0;
    int time_window_e=30;
    while (1)
    {
        clear;
        for(auto n:staff.notes)
        {
            add_note_to_time_excel(n);
        }
        clear;
        preview_staff(time_window_s,time_window_e);
        cout<<"按w向前移动时间窗口     按s向后移动时间窗口"<<endl;
        cout<<"按c添加一个note   按d删除一个note    "<<endl;
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
            staff.notes.push_back(n);

        }
        if(ch=='q')
        {
            break;
        }
    }
    
}

int main()
{
    clear;
    start_ide();
    return 0;
}
