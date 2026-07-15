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
        bool frame[100000][10][5];  // track 1-4, index 0 unused
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
            string zhuangtai[4]={" good"," miss"," bad ","     "};
            int good=0,miss=0,bad=0;
            Staff staff_copy=staff[staff_num];
            sort(staff_copy.notes.begin(),staff_copy.notes.end(),[](Note a,Note b){return a.etime<b.etime;});
            for(auto n:staff_copy.notes)
            {
                display.add_note_to_frame(n);
            }
            for(int i=0;i<staff_copy.time;i++)
            {

                //理论上只会有4押
                //so我大抵可以偷个懒

                // 只调用一次 get_key_nb，然后用循环收集按键
                char anjian[4];
                for(int k=0;k<4;k++) anjian[k]=get_key_nb();

                // need_tap[track]: 当前轨道需要击打的剩余tick数
                // note_idx[track]: 该轨道音符在 staff_copy.notes 中的索引
                int need_tap[4]={0,0,0,0};
                int note_idx[4]={-1,-1,-1,-1};
                int zt[4]={3,3,3,3};

                // 扫描前4个音符，建立轨道->音符的映射
                int scan_cnt=min(4,(int)staff_copy.notes.size());
                for(int j=0;j<scan_cnt;j++)
                {
                    int tr=staff_copy.notes[j].track-1;  // 0-based track
                    need_tap[tr]=staff_copy.notes[j].etime-i;
                    note_idx[tr]=j;
                }

                // 收集需要删除的音符索引（从大到小排序，方便安全删除）
                vector<int> to_erase;

                for(int j=0;j<4;j++)
                {
                    if(note_idx[j]<0) continue;  // 该轨道没有待处理音符

                    int dt=need_tap[j];

                    // miss: 已经错过判定窗口 (dt < -1)
                    if(dt<-1)
                    {
                        zt[j]=1;
                        miss++;
                        to_erase.push_back(note_idx[j]);
                        continue;
                    }

                    // 检查是否有对应轨道的按键按下
                    bool key_pressed=false;
                    for(int k=0;k<4;k++)
                    {
                        if(anjian[k]==push[j]) { key_pressed=true; break; }
                    }

                    if(!key_pressed) continue;  // 没按对应的键，跳过

                    // good: 在判定窗口内 (dt == -1, 0, 1)
                    if(dt>=-1 && dt<=1)
                    {
                        zt[j]=0;
                        good++;
                        to_erase.push_back(note_idx[j]);
                    }
                    // bad: 提前太早按了 (dt > 1 且 dt <= 3)
                    else if(dt>1 && dt<=3)
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
                    // dt > 3: 太早了，不算bad也不删除
                }

                // 从大到小排序索引，安全删除
                sort(to_erase.begin(),to_erase.end(),greater<int>());
                for(int idx:to_erase)
                {
                    staff_copy.notes.erase(staff_copy.notes.begin()+idx);
                }

                cout<<need_tap[0]<<" "<<need_tap[1]<<" "<<need_tap[2]<<" "<<need_tap[3]<<endl;
                cout<<note_idx[0]<<" "<<note_idx[1]<<" "<<note_idx[2]<<" "<<note_idx[3]<<endl;
                cout<<"good:"<<good<<" miss:"<<miss<<" bad:"<<bad<<endl;

                for(int j=0;j<4;j++)
                {
                    cout<<"|"<<zhuangtai[zt[j]];
                }
                cout<<"|         状态栏"<<endl;
                display.print_frame(i);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                clear;
            }
        }
        
}game;

int main()
{
    game.staff[0].name="test";
    game.staff[0].time=120;
    game.staff[0].notes.push_back({0,10,1});
    game.staff[0].notes.push_back({10,20,2});
    game.staff[0].notes.push_back({20,30,3});
    game.staff[0].notes.push_back({30,40,4});    
    game.staff[0].notes.push_back({50,60,1});
    game.staff[0].notes.push_back({60,70,2});
    game.staff[0].notes.push_back({70,90,3});
    game.staff[0].notes.push_back({90,100,4});
    game.start(0);
    return 0;
}