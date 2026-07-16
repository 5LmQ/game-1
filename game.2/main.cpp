#include "bits.h"

using namespace std;
#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    // Windows: _kbhit() 检测是否有键，非阻塞
    #define GETCH _getch()
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
            fin>>staff_tmp.name>>staff_tmp.time;
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
            cout<<"1。存入谱面"<<endl;
            cout<<"2。删除谱面"<<endl;
            cout<<"3。游玩谱面"<<endl;
            char a=GETCH;
            if(a=='1')
            {
                clear;
                cout<<"输入谱面文件地址(绝对路径)"<<endl;
                string file_path;
                cin>>file_path;
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
                    if(note_idx[tr] < 0)                 // 只取该轨道最早出现的音符，不覆盖
                    {
                        need_tap[tr]=staff_copy.notes[j].etime-i;
                        note_idx[tr]=j;
                    }
                }

                // 收集需要删除的音符索引（从大到小排序，方便安全删除）
                vector<int> to_erase;

                for(int j=0;j<4;j++)
                {
                    if(note_idx[j]<0) continue;  // 该轨道没有待处理音符

                    int dt=need_tap[j];

                    // miss: 已经错过判定窗口 (dt < -1)
                    if(dt<-2)
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
                    if(dt>=-2 && dt<=2)
                    {
                        zt[j]=0;
                        good++;
                        to_erase.push_back(note_idx[j]);
                    }
                    // bad: 提前太早按了 (dt > 1 且 dt <= 3)
                    else if(dt>2 && dt<=6)
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

                //容我记录一下我的发现，有部分音符由于我按的久会先后判定成good、miss，然后连续pop两个音符，使后面的音符无法判定

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
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                clear;
            }
        }
        
}game;

int main()
{
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