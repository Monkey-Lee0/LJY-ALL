#pragma GCC optimize("Ofast")
#include<bits/stdc++.h>
#include "ljyall.h"
using namespace std;
int main()
{
    // cerr<<"Choose your mode:"<<endl;
    // cerr<<"1.Interactive mode."<<endl;
    // cerr<<"2.code mode."<<endl;
    // int op;
    // cin>>op;
    // string s;
    // getline(cin,s);
    // if(op==1)
    // {
    //     int state=0;
    //     while(true)
    //     {
    //         try
    //         {
    //             if(!state)
    //                 cout<<">>> ";
    //             else
    //                 cout<<"... ";
    //             getline(cin,s);
    //             if(!state&&s=="begin")
    //                 state=1;
    //             if(state&&s=="end")
    //                 state=0;
    //             interpreter(s);
    //         }
    //         catch(invalid_expression &a)
    //         {
    //             cout<<"invalid_expression : ";
    //             puts(a.what());
    //         }
    //         catch(runtime_error &a)
    //         {
    //             cout<<"runtime_error : ";
    //             puts(a.what());
    //         }
    //         catch(domain_error &a)
    //         {
    //             cout<<"domain_error : ";
    //             puts(a.what());
    //         }
    //         catch(undefined_behavior &a)
    //         {
    //             cout<<"undefined_behavior : ";
    //             puts(a.what());
    //         }
    //         catch(bad_any_cast &a)
    //         {
    //             cout<<"bad_any_cast : ";
    //             puts(a.what());
    //         }
    //         catch(indentation_error &a)
    //         {
    //             cout<<"indentation_error : ";
    //             puts(a.what());
    //         }
    //         catch(...)
    //         {
    //             puts("What's wrong with the program (and your brain)?");
    //         }
    //     }
    // }
    // interpreter("begin");
    // while(true)
    // {
    //     getline(cin,s);
    //     interpreter(s);
    //     if(s=="end")
    //         break;
    // }
    freopen("1.in","r",stdin);
    freopen("1.out","w",stdout);
    interpreter("begin");
    std::string s;
    while(getline(cin,s))
        interpreter(s);
    interpreter("end");

    return 0;
}