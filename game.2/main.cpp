#include "bits.h"

using namespace std;
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    // Windows: _kbhit() 检测是否有键，非阻塞
    int get_key_nb() {
        if (_kbhit()) return _getch();
        return -1;   // 无按键
    }
    #define clear system("cls")
#else
    
    #include <termios.h>
    #include <unistd.h>
    #include <fcntl.h>
    // Linux/Mac: 把 stdin 设为非阻塞 + 非规范模式
    int get_key_nb() {
        struct termios oldt, newt;
        tcgetattr(STDIN_FILENO, &oldt);
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);  // 非阻塞

        int ch = getchar();
        if (ch == EOF) ch = -1;   // 无按键

        fcntl(STDIN_FILENO, F_SETFL, flags);   // 恢复原 flags
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        return ch;
    }
    #define clear system("clear")
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
        bool frame[100000][10][4];
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
        }
}display;


class Game
{
    public:
        Staff staff[100];
        void start(int staff_num)
        {
            Staff staff_copy=staff[staff_num];
            sort(staff_copy.notes.begin(),staff_copy.notes.end(),[](Note a,Note b){return a.etime<b.etime;});
            vector<Note> near_note;
            for(auto n:staff_copy.notes)
            {
                display.add_note_to_frame(n);
            }
            for(int i=0;i<staff_copy.time;i++)
            {
                display.print_frame(i);
                char anjian[4];
                anjian[0]=get_key_nb();
                anjian[1]=get_key_nb();
                anjian[2]=get_key_nb();
                anjian[3]=get_key_nb();
                for(int j=0;j<4;j++)
                {
                    cout<<"按下"<<anjian[j]<<endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                clear;
            }
        }
        
}game;

int main()
{
    game.staff[0].name="test";
    game.staff[0].time=100;
    game.staff[0].notes.push_back({0,10,1});
    game.staff[0].notes.push_back({10,20,2});
    game.staff[0].notes.push_back({20,30,3});
    game.staff[0].notes.push_back({30,40,4});
    game.start(0);
    return 0;
}