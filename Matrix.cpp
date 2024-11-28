#include<bits/stdc++.h>
#include "Fraction.h"
using namespace std;
Fraction<int> a[500][1000];
int n;
int solve(int x){return x>n?x-n:x;}
int main()
{
    int T;
    cin>>T;
    while(T--)
    {
        cin>>n;
        for(int i=1;i<=n;i++)
        {
            int pos=solve(n+2-i);
            for(int j=1;j<=n;j++)
                a[i][j]=Fraction(pos),pos=solve(pos+1);
            for(int j=n+1;j<=n+n;j++)
                a[i][j]=Fraction(0);
            a[i][i+n]=Fraction(1);
        }
        for(int i=1;i<=n;i++)
        {
            int t=0;
            for(int j=i;j<=n;j++)
                if(abs(a[j][i])>=abs(a[t][i]))
                    t=j;
            if(!a[t][i])
            {
                printf("No Solution\n");
                return 0;
            }
            swap(a[i],a[t]);
            for(int j=1;j<=n;j++)
            {
                if(i==j)
                    continue;
                auto num=a[j][i]/a[i][i];
                for(int k=1;k<=2*n;k++)
                    a[j][k]-=a[i][k]*num;
            }
        }
        for(int i=1;i<=n;i++)
        {
            for(int j=n+1;j<=2*n;j++)
                cout<<a[i][j]/a[i][i]<<" ";
            cout<<endl;
        }
    }


    return 0;
}