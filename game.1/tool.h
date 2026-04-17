#include <bits/stdc++.h>

using namespace std;

struct TOOL
{
    template<size_t N>
    void char_dependent_segmentation(string (&ans)[N],string in,char a)//依照a分割字符串in,存入ans
    {
        int w=0;
        for(char y:in)
        {
            if(y==a)
            {
                w++;
            }
            else
            {
                ans[w]+=y;
            }
        }
    }

    template<size_t N>
    void str_to_int(int (&ans)[N],string in[N])//将字符串数组转换为int数组
    {
        for(int i=0;i<N;i++)
        {
            if(in[i]=="")
            {
                ans[i]=0;
                continue;
            }
            ans[i]=stoi(in[i]);
        }
    }


}tool;
