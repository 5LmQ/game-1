#include "bits.h"

using namespace std;
#ifdef _WIN32
#define clear system("cls")
#define GETCH _getch()
#include <conio.h>
#include <windows.h>
#else
    #define clear system("clear")
    #include <termios.h>
    #include <unistd.h>
    // Linux/Mac 无回显读取
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
    Note notes[10000];
};
class Dispaly
{
    public:
        bool frame[100000][10][4];
        string px[2]={"     ","-----"};
        
        void add_note_to_frame(Note n)
        {
            for(int i=n.stime;i<=n.etime;i++)
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
            cout<<"-------------------------"<<endl;
        }

        void video_play()
        {
            for(int i=0;i<=25;i++)
            {
                print_frame(i);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                clear;
            }
        }
}display;


class Game
{
    public:
        Staff staff[100];
        
};

int main()
{
    Note n[3]={
        {0,10,1},
        {5,15,2},
        {0,20,3}
    };
    display.add_note_to_frame(n[0]);
    display.add_note_to_frame(n[1]);
    display.add_note_to_frame(n[2]);
    display.video_play();
    return 0;
}