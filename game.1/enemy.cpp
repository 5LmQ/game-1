#include <bits/stdc++.h>

using namespace std;

class Enemy // 敌人总基类
{
public:

    struct enemykind
    {
        string name;
        int bl_v;
        int enemy_at_power;
    };
    
    enemykind kind[1000]={{"      natural",200,50},
                          {},
                        };

    int Enemy_Unit_Combination[10000][10]={{1,1,1,1,1}};

    int e[100],at_power[100];
    string enemy_name[100];
    void emy(int en_co)//敌方初始化
    {
        for(int i=1;i<=5;i++)
        {
            enemy_name[i]=kind[Enemy_Unit_Combination[en_co-1][i-1]-1].name;
            e[i]=kind[Enemy_Unit_Combination[en_co-1][i-1]-1].bl_v;
            at_power[i]=kind[Enemy_Unit_Combination[en_co-1][i-1]-1].enemy_at_power;
        }
    }
    void print()//显示敌方状态
    {
        cout<<"                                      敌方状态                               "<<endl;
        cout<<"      -----------------------------------------------------------------------"<<endl;
        cout<<"      |             |             |             |             |             |"<<endl;
        cout<<"  名字";
        for(int i=1;i<=5;i++)
        {
            cout<<"|";
            if(e[i]==0)
            {
                printf("             ");
                continue;
            }
            cout<<enemy_name[i];
            //printf("%13s",enemy_name[i]);
        }
        cout<<"|"<<endl;
        cout<<"生命值";
        for(int i=1;i<=5;i++)
        {
            cout<<"|";
            if(e[i]==0)
            {
                //string s="消亡";
                cout<<"         消亡";
                continue;
            }
            printf("%13d",e[i]);
        }
        cout<<"|"<<endl;
        cout<<"攻击力";
        for(int i=1;i<=5;i++)
        {
            cout<<"|";
            if(e[i]==0)
            {
                printf("             ");
                continue;
            }
            printf("%13d",at_power[i]);
        }
        cout<<"|"<<endl;
        cout<<"  位置|      1      |      2      |      3      |      4      |      5      |"<<endl;
        cout<<"      -----------------------------------------------------------------------"<<endl;
    }
    int ifkia()//全灭判断
    {
        for(int i=1;i<=5;i++)
        {
            if(e[i])
            {
                return 0;
            }
        }
        return 1;
    }
private:
};

Enemy enemy;//敌人对象

class Ordinary_Enemy : public Enemy   //普通敌人
{
public:
    virtual void attack_1(int* player_data,int bianhao) = 0;//第一种普攻
    virtual void attack_2(int* player_data,int bianhao) = 0;//第二种普攻
};
class Elite_Enemy : public Enemy   //精英敌人
{
public:
    virtual void attack_1(int* player_life) = 0;//第一种普攻
    virtual void attack_2(int* player_life) = 0;//第二种普攻
    virtual void eattack(int* player_life) = 0;//e技能
    virtual void generate() = 0;//召唤
    virtual void qattacking(int* player_life) = 0;//大招蓄力
    virtual void qattack(int* player_life) = 0;//大招
};
class Boss : public Enemy   //boss or boss的一个形态
{
public:
    virtual void attack_1(int* player_life) = 0;//第一种普攻
    virtual void attack_2(int* player_life) = 0;//第二种普攻
    virtual void eattack_1(int* player_life) = 0;//第一种e技能
    virtual void eattack_2(int* player_life) = 0;//第二种e技能
    virtual void eattack_3(int* player_life) = 0;//第三种e技能
    virtual void strenger(int* player_life) = 0;//强化
    virtual void generate() = 0;//召唤
    virtual void qattacking(int* player_life) = 0;//大招蓄力
    virtual void qattack(int* player_life) = 0;//大招
};

class Natural_Number : public Ordinary_Enemy
{
public:
    void attack_1(int* player_data,int bianhao) override {//从玩家血量中选择最大的两个数，拼和成一个百分数，此次攻击所造成的的伤害为攻击力与这个百分数乘算的结果
        int p_l = *player_data;
        // 步骤1：提取玩家血量的每一位数字
        int every_b_l[5] = {0}; 
        int digit_count = 0;    // 记录实际提取的数字位数
        int temp = p_l;

        // 分解玩家血量的每一位数字
        while (temp > 0 && digit_count < 5) 
        {
            every_b_l[digit_count++] = temp % 10; // 取个位数字
            temp = temp / 10;                     // 去掉个位
        }

        int num1, num2;
            // 对提取的数字降序排序
            std::sort(every_b_l, every_b_l+5, std::greater<int>());
            num1 = every_b_l[0]; // 最大的数字
            num2 = every_b_l[1]; // 第二大的数字
      //  cout<<num1<<endl<<num2<<endl;

        // 步骤3：拼接成百分数（比如num1=9, num2=5 → 95% → 0.95）
        double percentage = (num1 * 10 + num2) / 100.0;
        percentage*=enemy.at_power[bianhao];
        *player_data-=percentage;
        cout<<"-"<<percentage<<endl;
    }
    void attack_2(int* player_date,int bianhao) override{//从玩家能量中选择最大的一个数，拼和成一个百分数，此次攻击会吸取玩家能量，大小为攻击力与这个百分数乘算的结果，此次攻击后玩家不会获得手机
        int p_l = *player_date;
        // 步骤1：提取玩家能量的每一位数字
        int every_b_l[5] = {0}; // 初始化数组，最多存储5位数字
        int digit_count = 0;    // 记录实际提取的数字位数
        int temp = p_l;

        // 分解玩家能量的每一位数字
        while (temp > 0 && digit_count < 5) 
        {
            every_b_l[digit_count++] = temp % 10; // 取个位数字
            temp = temp / 10;                     // 去掉个位
        }

        int num1;
        // 对提取的数字降序排序
        std::sort(every_b_l, every_b_l + 5, std::greater<int>());
        num1 = every_b_l[0]; // 最大的数字
        // 步骤3：拼接成百分数（比如num1=9, num2=5 → 95% → 0.95）
        double percentage = (num1) / 100.0;
        *player_date-=percentage*enemy.at_power[bianhao];
        cout<<"-"<<percentage*enemy.at_power[bianhao]<<endl;
    }
};
