#include<bits/stdc++.h>
#include"int2048.h"
#define mod 1000000007
#define maxn 120
using namespace std;
struct Matrix
{
    int n,m;
    int65536 a[maxn][maxn];
    Matrix(){n=m=0;}
    Matrix operator *(Matrix B)
    {
        Matrix C;
        C.n=n;C.m=B.m;
        for(int i=1;i<=n;i++)
            for(int j=1;j<=B.m;j++)
                for(int k=1;k<=m;k++)
                    C.a[i][j]+=a[i][k]*B.a[k][j];
        return C;
    }
};
Matrix P,Q;
Matrix power(Matrix A,long long y)
{
    Matrix ans,lazy=A;
    ans.n=ans.m=P.n;
    for(int i=1;i<=P.n;i++)
        ans.a[i][i]=int65536(1);
    while(y)
    {
        if(y&1)
            ans=ans*lazy;
        lazy=lazy*lazy;
        y>>=1;
    }
    return ans;
}
long long k;
int main()
{
    scanf("%d",&P.n);
    P.m=P.n;
    for(int i=1;i<=P.n;i++)
        for(int j=1;j<=P.n;j++)
            cin>>P.a[i][j];
    Q.m=Q.n=P.n;
    for(int i=1;i<=Q.n;i++)
        for(int j=1;j<=Q.n;j++)
            cin>>Q.a[i][j];
    P=P*Q;
    for(int i=1;i<=P.n;i++)
    {
        for(int j=1;j<=P.n;j++)
            cout<<P.a[i][j]<<" ";
        puts("");
    }


    return 0;
}