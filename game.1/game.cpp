#include <bits/stdc++.h>
#include "tool.h"
#include "enemy.cpp"

using namespace std;
#ifdef _WIN32
#define CLEAR_SCREEN system("cls")
#define GETCH _getch()
#include <conio.h>
#include <windows.h>
#else
    #define CLEAR_SCREEN system("clear")
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

int role_att_power[10]={100};
int role_life[10]={1500};

class The_lost//玩家
{
public:

    int att_power;
    int lostnl,life;
    string name="the lost";
    bool recovery_state=false; // 「回复」状态标记
    int recovery_turn=0;       // 「回复」状态剩余回合
    int damage_record=0;       // 「回复」状态下记录的伤害值
    int rationality=0;//理性点，普攻后增加，消耗以发动e技能，理智点属于各个角色独有，不与其他角色共享
    The_lost()
    {
        att_power=role_att_power[0];
        lostnl=0;
        life=role_life[0];
        name="the lost";
        recovery_state=false;
        recovery_turn=0;
        damage_record=0;
        rationality=0;
    }
    void lostaskill(int d,int *lostnl,int *e,int *rationality)//普攻
    {
        *lostnl=*lostnl+20;
        int damage=0.8*att_power;
        e[d]-=damage;
        if(recovery_state) {
            damage_record += damage;
            cout<<"【回复状态】记录普攻伤害："<<damage<<" 累计："<<damage_record<<endl;
        }
        *rationality+=1;
        if(e[d] <= 0) 
        {
            e[d]=0;
        }
    }
    void losteskill(int d,int *lostnl,int *e,int *rationality)//e技能
    {
        if(*rationality>0)
        {
            *rationality-=1;
        }
        else
        {
            cout<<"理性不足"<<endl;
            return;
        }
        int damage=2*att_power;
        e[d]-=damage;
        if(recovery_state) {
            damage_record += damage;
            cout<<"【回复状态】记录E技能伤害："<<damage<<" 累计："<<damage_record<<endl;
        }
        *lostnl+=30;
        if(e[d] <= 0)
        {
            e[d]=0;
        }
    }

    void lostqskill(int *lostnl)//大招
    {
        if(*lostnl>=qenergy)
        {
            *lostnl-=qenergy;
        }
        else
        {
            cout<<"能量不足"<<endl;
            return;
        }

        // 大招核心逻辑：回复2个理性点，进入回复状态（持续2回合），重置伤害记录
        rationality += 2;
        recovery_state=true;
        recovery_turn=3+1;
        damage_record=0;
        cout<<"【Q技能生效】回复2个理性点（当前："<<rationality<<"），进入「回复」状态，持续3回合！"<<endl;

        // 回复状态结束处理（如果是状态到期）- 此处由回合逻辑触发，这里仅初始化状态
        *lostnl+=30;
    }

    //每回合结束时调用，更新回复状态
    void update_recovery_state() {
        if(recovery_state) 
        {
            int damage_record_per=0.5;
            recovery_turn--;
            cout<<"【回复状态】剩余回合："<<recovery_turn<<endl;
            int heal=damage_record*damage_record_per;
            damage_record*=damage_record_per;
            life += heal;
            // 生命值不超过上限
            if(life > 1500) life=1500;
            cout<<"【回复状态】累计造成伤害："<<damage_record<<"，转化"<<heal<<"点记录值（当前："<<life<<"）"<<endl;
	     if(recovery_turn==0)
            {
                recovery_state=false;
                damage_record=0;
            } 
        }
    }
private:
    int qenergy=180;//e技能能量、q技能能量
};

    string screen_stirp[20]={"==============================================================",//0     //界面显示条，长最长60+2字符
                             "|                                                            |",//1
                             "|                          wating.                           |",//2
                             "|                          wating..                          |",//3
                             "|                          wating...                         |",//4
                             "|          ","          |",//小型区（10+1）
                              //     5            6
                             "     "};//mini款
                             // 7

The_lost lost;
int bag[100000];
string storage_breakdown[100000]={
    "金币",
    "升级书"
};

string spoil[100000]={"50.5"};


class operate//操作类
{
public:

    string operate_log[10000];

    void start_fight()
    {
        while(1)
        {
            int copy_o=0;
            cout<<"=============================================================="<<endl;
            cout<<"|                          选择关卡                          |"<<endl;
            cout<<"| -----                                                      |"<<endl;
            cout<<"| | 1 |                                                      |"<<endl;
            cout<<"| -----                                                      |"<<endl;
            cout<<"|                          按q退出                           |"<<endl;
            cout<<"=============================================================="<<endl;
            string a;
            a=GETCH;
            if(a=="q")
            {
                CLEAR_SCREEN;
                break;
            }
            enemy.emy(stoi(a));
            while(1)
            {
                CLEAR_SCREEN;
                cout<<"选择第"<<a<<"关"<<endl;
                cout<<"按s开始       按c查看详情       按k修改行为复制是否开启，当前：";
                if(copy_o)
                {
                    cout<<"■"<<endl;
                }
                else
                {
                    cout<<"□"<<endl;
                }
                cout<<"按q退出"<<endl;
                string o;
                o=GETCH;
                if(o=="s")
                {
                    CLEAR_SCREEN;
                    select(stoi(a));
                }
                else if(o=="c")
                {
                    CLEAR_SCREEN;
                    cout<<"关卡"<<a<<"详情："<<endl;
                    enemy.print();

                    int give[1000];
                    string strgive[1000];
                    tool.char_dependent_segmentation(strgive,spoil[stoi(a)-1],'.');
                    tool.str_to_int(give,strgive);
                    cout<<"奖励："<<endl;
                    for(int i=0;i<10;i++)
                    {
                        if(give[i])
                        {
                            cout<<screen_stirp[7]<<storage_breakdown[i]<<":"<<give[i]<<endl;
                        }
                    }

                    cout<<"按任意键返回"<<endl;
                    GETCH;
                    CLEAR_SCREEN;
                }
                else if(o=="k")
                {
                    copy_o=!copy_o;
                    CLEAR_SCREEN;
                }
                else if(o=="q")
                {
                    CLEAR_SCREEN;
                    break;
                }
            }
            
        }
        return ;

    }

    int generate(int level)//生成敌方
    {
        cout<<"战斗开始"<<endl;
        enemy.emy(level);
        enemy.print();
        return 0;
    }
    queue <string>actionqueue;//行动队列
    queue <int>numberqueue;//编号队列
    //行动队列中的单位会轮换行动
    int select(int number_of_level)//选择关卡
    {
        generate(number_of_level);
        for(int i=1;i<=1;i++)
        {
            string rolename="lost"; 
            actionqueue.push(rolename);
            numberqueue.push(i);
        }

        for(int i=1;i<=5;i++)
        {
            actionqueue.push(enemy.enemy_name[i]);
            numberqueue.push(i);
        }

        int if_vitcter=0;
        while (1)//回合轮换
        {
           // int o;
            //cin>>o;//---------------------------------------------------------------------------------------------------------------------------------------------------------
            string ppp=actionqueue.front();
            int a=numberqueue.front();
            move(ppp);
            actionqueue.pop();
            numberqueue.pop();
            actionqueue.push(ppp);
            numberqueue.push(a);
            if(enemy.ifkia())
            {
                if_vitcter=1;
                break;
            }
            else if(lost.life<=0)
            {
                if_vitcter=0;
                break;
            }
        }
        if(if_vitcter)
        {
            cout<<"战斗胜利"<<endl;
            lost.att_power=role_att_power[0];
            lost.lostnl=0;
            lost.life=role_life[0];
            lost.name="the lost";
            lost.recovery_state=false;
            lost.recovery_turn=0;
            lost.damage_record=0;
            lost.rationality=0;
            int give[1000];
            string strgive[1000];
            tool.char_dependent_segmentation(strgive,spoil[number_of_level-1],'.');
            tool.str_to_int(give,strgive);
            cout<<"获得："<<endl;
            for(int i=0;i<10;i++)
            {
                if(give[i])
                {
                    cout<<screen_stirp[7]<<storage_breakdown[i]<<":"<<give[i]<<endl;
                    bag[i]+=give[i];
                }
            }
        }
        else
        {
            cout<<"战斗失败"<<endl;
            lost.att_power=role_att_power[0];
            lost.lostnl=0;
            lost.life=role_life[0];
            lost.name="the lost";
            lost.recovery_state=false;
            lost.recovery_turn=0;
            lost.damage_record=0;
            lost.rationality=0;

            
        }
        return 0;
    }
    int move(string role)
    {
        if(role=="lost")
        {
            cout<<" 选择操作  当前能量："<<lost.lostnl<<"       理性点："<<lost.rationality<<"          生命值："<<lost.life<<endl;
            cout<<"a：普攻   e：技能   q：大招"<<endl;
            char operationoptions;
            operationoptions=GETCH;
            if(operationoptions=='a')
            {
                int d;
                CLEAR_SCREEN;
                enemy.print();
                cout<<"发动普攻，请选择目标1~5:"<<endl;
                char d_char=GETCH;
                d=d_char - '0';
                //d-=1;
                lost.lostaskill(d,&lost.lostnl,enemy.e,&lost.rationality);
                CLEAR_SCREEN;
                enemy.print();
                cout<<endl;
            }
            if(operationoptions=='e')
            {
                int d;
                CLEAR_SCREEN;
                enemy.print();
                cout<<"发动技能，请选择目标(1~5):"<<endl;
                int d_char=GETCH;
                d=d_char - '0';
                //d-=1;
                enemy.print();
                lost.losteskill(d,&lost.lostnl,enemy.e,&lost.rationality);
                CLEAR_SCREEN;
                enemy.print();
                cout<<endl;
            }
            if(operationoptions=='q')
            {
                //enemy.print();
                lost.lostqskill(&lost.lostnl);
                CLEAR_SCREEN;
                enemy.print();
                cout<<endl;
            }
            lost.update_recovery_state();
        }
        else if(role=="      natural"&&enemy.e[numberqueue.front()]>0)
        {
            Natural_Number natural_number;
            CLEAR_SCREEN;
            enemy.print();
            unsigned int seed=chrono::system_clock::now().time_since_epoch().count();
            mt19937 engine(seed);
            uniform_int_distribution<int> int_dist(1, 4);
            int random_int=int_dist(engine);
            cout<<"敌方"<<numberqueue.front()<<"号敌人，natural_number发动技能:"<<endl;
            if(180-lost.lostnl<10)
            {
                if(random_int>=2)
                {
                    natural_number.attack_2(&lost.lostnl,numberqueue.front());
                    cout<<"attack_2"<<endl;
                    cout<<"抽取了玩家能量,玩家能量剩余："<<lost.lostnl<<endl;
                }
                else
                {
                    natural_number.attack_1(&lost.life,numberqueue.front());
                    cout<<"attack_1"<<endl;
                    cout<<"攻击玩家,玩家生命剩余："<<lost.life<<endl;
                }
            }
            else
            {
                if(random_int==1)
                {
                    natural_number.attack_2(&lost.lostnl,numberqueue.front());
                    cout<<"attack_2"<<endl;
                    cout<<"抽取了玩家能量,玩家能量剩余："<<lost.lostnl<<endl;
                }
                else
                {
                    natural_number.attack_1(&lost.life,numberqueue.front());
                    cout<<"attack_1"<<endl;
                    cout<<"攻击玩家,玩家生命剩余："<<lost.life<<endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            CLEAR_SCREEN;
            enemy.print();
            
        }
        return 0;
    }
    
    
    //养成

    void yangcheng2(string name)
    {
        while (1)
        {
            cout<<screen_stirp[0]<<endl;
            cout<<"所选角色:"<<name<<endl;
            cout<<screen_stirp[5]<<"1.升级等级                                 "<<screen_stirp[6]<<endl;
            cout<<screen_stirp[5]<<"2.升级天赋                                 "<<screen_stirp[6]<<endl;
            cout<<screen_stirp[5]<<"3.升级装备                                 "<<screen_stirp[6]<<endl;
            cout<<screen_stirp[5]<<"4.额外技能树                               "<<screen_stirp[6]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[0]<<endl;
            cout<<"输入序号1~4      q：退出"<<endl;
            char operationoptions;
            operationoptions=GETCH;
            if(operationoptions=='q')
            {
                return;
            }
            else if(operationoptions=='1')
            {
                //开发中.ing
                
            }
        }
    } 

    void yangcheng1()
    {
        string nameye[10][10]={{"lost(1)"}};
        // 显示角色
        string name;
        int page=1;
        int max_page=2;
        while (1)
        {
            cout<<screen_stirp[0]<<endl;
            cout<<"|所有角色:                                                   |"<<endl;
            cout<<screen_stirp[5]<<"1."<<nameye[page-1][0]<<"                                  "<<screen_stirp[6]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[0]<<endl;
            cout<<"输入角色序号1~9      z：上一页     x：下一页     q：退出"<<endl;
            name=GETCH;
            if(name=="z")
            {
                if(page==1)
                {
                    page=max_page;
                }
                page--;
            }
            else if(name=="x")
            {
                if(page==max_page)
                {
                    page=1;
                }
                page++;
            }
            else if(name=="q")
            {
                CLEAR_SCREEN;
                return ;
            }
            else if(name>="1"&&name<="9")
            {
                yangcheng2(nameye[page-1][stoi(name)-1]);
            }
        }
    }

};

operate o;

class display//显示类
{
public:
//------------------------------------------------------------------------游戏开始显示
    struct AnimationFrame {
        string text;
        string progress_bar;
    };
    // 预定义动画帧
    vector<AnimationFrame> loading_frames={
        {screen_stirp[2], "□□□□□□□□□□□□□□□□□□□□"},
        {screen_stirp[3], "■□□□□□□□□□□□□□□□□□□□"}, 
        {screen_stirp[4], "■■□□□□□□□□□□□□□□□□□□"}, 
        {screen_stirp[2], "■■■□□□□□□□□□□□□□□□□□"}, 
        {screen_stirp[3], "■■■■□□□□□□□□□□□□□□□□"}, 
        {screen_stirp[4], "■■■■■□□□□□□□□□□□□□□□"}, 
        {screen_stirp[2], "■■■■■■□□□□□□□□□□□□□□"}, 
        {screen_stirp[3], "■■■■■■■□□□□□□□□□□□□□"}, 
        {screen_stirp[4], "■■■■■■■■□□□□□□□□□□□□"}, 
        {screen_stirp[2], "■■■■■■■■■□□□□□□□□□□□"}, 
        {screen_stirp[3], "■■■■■■■■■■□□□□□□□□□□"}, 
        {screen_stirp[4], "■■■■■■■■■■■□□□□□□□□□"}, 
        {screen_stirp[2], "■■■■■■■■■■■■□□□□□□□□"}, 
        {screen_stirp[3], "■■■■■■■■■■■■■□□□□□□□"}, 
        {screen_stirp[4], "■■■■■■■■■■■■■■□□□□□□"}, 
        {screen_stirp[2], "■■■■■■■■■■■■■■■□□□□□"}, 
        {screen_stirp[3], "■■■■■■■■■■■■■■■■□□□□"}, 
        {screen_stirp[4], "■■■■■■■■■■■■■■■■■□□□"}, 
        {screen_stirp[2], "■■■■■■■■■■■■■■■■■■□□"}, 
        {screen_stirp[3], "■■■■■■■■■■■■■■■■■■■□"}, 
        {screen_stirp[4], "■■■■■■■■■■■■■■■■■■■■"}


    };
    // 渲染单个帧的通用方法
    void render_frame(const AnimationFrame& frame) {
        cout<<screen_stirp[0]<<endl<<screen_stirp[1]<<endl;
        cout<<screen_stirp[1]<<endl<<screen_stirp[1]<<endl;
        cout<<screen_stirp[1]<<endl<<screen_stirp[1]<<endl;
        cout <<frame.text<<endl;
        cout<<screen_stirp[5] + screen_stirp[7] + screen_stirp[7];
        cout<<frame.progress_bar;
        cout<<screen_stirp[7] + screen_stirp[7] + screen_stirp[6]<<endl;
        cout<<screen_stirp[1]<<endl<<screen_stirp[0]<<endl;
    }
    
    int start_screen()//开始屏幕显示
    {
        int a;
        cout<<screen_stirp[0]<<endl<<screen_stirp[1]<<endl;
        cout<<screen_stirp[5]<<"--                                    --"<<screen_stirp[6]<<endl;
        cout<<screen_stirp[5]<<"(            名为数学的游戏            )"<<screen_stirp[6]<<endl;
        cout<<screen_stirp[5]<<"--                                    --"<<screen_stirp[6]<<endl<<screen_stirp[1]<<endl;
        cout<<screen_stirp[5]<<"        (输入任意内容以开始游戏)        "<<screen_stirp[6]<<endl<<screen_stirp[1]<<endl<<screen_stirp[1]<<endl<<screen_stirp[0]<<endl;
        cout<<"输入任意内容以开始游戏：";
        cin>>a;
        cout<<endl;
        cout<<"欢迎来到“名为数学的游戏”游戏马上开始"<<endl;
        
        // 播放加载动画
        for (const auto& frame : loading_frames) {
            render_frame(frame);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
        
        return 0;
    }
//------------------------------------------------------------------------游戏开始显示
//------------------------------------------------------------------------角色 or 战斗

    int display_role()
    {
        string nameye[10][10]={{"lost(1)"}};
        // 显示角色
        string name;
        int page=1;
        int max_page=2;
        while (1)
        {
            cout<<screen_stirp[0]<<endl;
            cout<<"|所有角色:                                                 |"<<endl;
            cout<<screen_stirp[5]<<"1."<<nameye[page-1][0]<<"                                  "<<screen_stirp[6]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[0]<<endl;
            cout<<"输入角色序号1~9      z：上一页     x：下一页     q：退出"<<endl;
            name=GETCH;
            if(name=="z")
            {
                if(page==1)
                {
                    page=max_page;
                }
                page--;
            }
            else if(name=="x")
            {
                if(page==max_page)
                {
                    page=1;
                }
                page++;
            }
            else if(name=="q")
            {
                CLEAR_SCREEN;
                return 0;
            }
            else if(nameye[page-1][stoi(name)-1]=="lost(1)")
            {
                CLEAR_SCREEN;
                cout<<"角色名称："<<nameye[page-1][stoi(name)-1]<<endl;
                cout<<"角色技能："<<endl;
                cout<<screen_stirp[7] + "天赋：探索者   形态：1"<<endl;
                cout<<screen_stirp[7] + screen_stirp[7] + "天赋：    作为独行于此的你，步步为艰，你将探索此地，学习便是你最好的武器，（经过学习你将会获得新的一套技能）"<<endl;
                cout<<screen_stirp[7] + "形态：无知    初来乍到，你一无所知"<<endl;
                cout<<screen_stirp[7] + screen_stirp[7] + "普攻a：   回复一个理性点，对指定敌方单体造成80%攻击力的伤害，回复20能量"<<endl;
                cout<<screen_stirp[7] + screen_stirp[7] + "E技能e：  消耗一个理性点，对指定敌方单体造成200%攻击力的伤害，回复30能量"<<endl;
                cout<<screen_stirp[7] + screen_stirp[7] + "Q技能q：  消耗180能量，回复两个理性点，进入「回复」状态，持续三回合，此状态下记录角色造成的伤害，在状态中，每回合结束后将记录的伤害的50%转化为血量回复量"<<endl;
                cout<<"按任意键以返回"<<endl;
                string returnq;
                returnq=GETCH;
            }
            CLEAR_SCREEN;
        }
    }

    int choose()
    {
        while(1)
        {
            cout<<screen_stirp[0]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<"|                      输入“c”查看角色                       |"<<endl;
            cout<<"|                      输入“f”开始战斗                       |"<<endl;
            cout<<"|                      输入“g”养成角色                       |"<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[1]<<endl;
            cout<<screen_stirp[0]<<endl;
            string a;
            a=GETCH;
            if(a=="c")
            {
                CLEAR_SCREEN;
                display_role();
            }
            else if(a=="f")
            {
                CLEAR_SCREEN;
                o.start_fight();
            }   
            else if(a=="g")
            {
                CLEAR_SCREEN;
                o.yangcheng1();   
            }
            else
            {
                CLEAR_SCREEN;
            }
        }
    }
    

};

int main()
{

    #ifdef _WIN32
        SetConsoleOutputCP(65001);  // Windows GBK
        SetConsoleCP(65001);
    #else
        setlocale(LC_ALL, "");    // Linux/Mac UTF-8
    #endif

    display screen;
    //screen.start_screen();//不知道为啥，把注释解了就会出事
    screen.choose();
    // operate fight;
    // fight.generate();
    // fight.select(1);//只有一个角色
    return 0;
}