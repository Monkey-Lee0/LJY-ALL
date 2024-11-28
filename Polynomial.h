#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H
#include "intelligent.h"

using std::gcd;
using std::lcm;

#include<utility>
#include<vector>
#include<exception>
#include<algorithm>
#include<stack>

constexpr int MAX_INDEX=5e7;

template<class T> class Polynomial;

template<class T> Polynomial<T> operator+ (const Polynomial<T> &a,const Polynomial<T> &b)
{
    Polynomial<T> c;
    int pos_a=0,pos_b=0;
    while(pos_a!=a.pol.size()||pos_b!=b.pol.size())
    {
        if(pos_a!=a.pol.size()&&pos_b!=b.pol.size())
        {
            if(a.pol[pos_a].first<b.pol[pos_b].first)
                c.pol.emplace_back(a.pol[pos_a++]);
            else if(a.pol[pos_a].first>b.pol[pos_b].first)
                c.pol.emplace_back(b.pol[pos_b++]);
            else
                c.pol.emplace_back(a.pol[pos_a].first,a.pol[pos_a].second+b.pol[pos_b].second),pos_a++,pos_b++;
        }
        else if(pos_b==b.pol.size())
            c.pol.emplace_back(a.pol[pos_a++]);
        else
            c.pol.emplace_back(b.pol[pos_b++]);
    }
    c.simplify();
    return c;
}
template<class T> Polynomial<T> operator+ (const Polynomial<T> &a){return a;}
template<class T> Polynomial<T> operator- (const Polynomial<T> &a)
{
    Polynomial<T> b=a;
    for(auto &x:b.pol)
        x.second=-x.second;
    return b;
}
template<class T> Polynomial<T> operator- (const Polynomial<T> &a,const Polynomial<T> &b){return a+(-b);}
template<class T> Polynomial<T> operator* (const Polynomial<T> &a,const Polynomial<T> &b)
{
    Polynomial<T> c;
    for(auto [exp1,fac1]:a.pol)
    {
        Polynomial<T> d;
        for(auto [exp2,fac2]:b.pol)
            d.pol.emplace_back(exp1+exp2,fac1*fac2);
        c=c+d;
    }
    return c;
}
template<class T> Polynomial<T> operator* (const Polynomial<T> &a,const T &b)
{
    Polynomial<T> c;
    for(auto [exp1,fac1]:a.pol)
        c.pol.emplace_back(exp1,fac1*b);
    c.simplify();
    return c;
}
template<class T> Polynomial<T> operator* (const T &a,const Polynomial<T> &b){return b*a;}
template<class T> Polynomial<T> operator/ (const Polynomial<T> &a,const T &b)
{
    if(!b)
        throw std::runtime_error("The divisor can't be zero!");
    Polynomial<T> c;
    for(int i=0;i<static_cast<int>(a.pol.size());i++)
    {
        T q=a.pol[i].second/b;
        if(a.pol[i].first&&q*b!=a.pol[i].second)
            throw std::runtime_error("Illegal division!");
        c.pol.emplace_back(a.pol[i].first,q);
    }
    c.simplify();
    return c;
}
template<class T> Polynomial<T> operator/ (const Polynomial<T> &a,const Polynomial<T> &b)
{
    if(b.pol.empty())
        throw std::runtime_error("The divisor can't be zero!");
    if(b.pol.size()==1&&b.pol[0].first==0)
        return a/b.pol[0].second;
    Polynomial<T> c=a,ans;
    while(!c.pol.empty()&&c.pol.back().first>=b.pol.back().first)
    {
        Polynomial<T> tmp;
        tmp.pol.emplace_back(c.pol.back().first-b.pol.back().first,c.pol.back().second/b.pol.back().second);
        c=c-b*tmp;
        if(c.pol.back().first==tmp.pol[0].first+b.pol.back().first)
            throw std::runtime_error("Illegal division!");
        ans=ans+tmp;
    }
    return ans;
}
template<class T> Polynomial<T> operator% (const Polynomial<T> &a,const T &b)
{
    if(!b)
        throw std::runtime_error("The modulus can't be zero!");
    try{return a-a/b*b;}
    catch(...){throw std::runtime_error("Illegal modulo!");}
}
template<class T> Polynomial<T> operator% (const Polynomial<T> &a,const Polynomial<T> &b)
{
    if(b.pol.empty())
        throw std::runtime_error("The modulus can't be zero!");
    if(b.pol.size()==1&&b.pol[0].first==0)
        return a%b.pol[0].second;
    try{return a-a/b*b;}
    catch(...){throw std::runtime_error("Illegal modulo!");}
}
template<class T> Polynomial<T> operator^ (Polynomial<T> a,int b)
{
    Polynomial<T> ans(T(1));
    while(b)
    {
        if(b&1)
            ans=ans*a;
        a=a*a;
        b>>=1;
    }
    return ans;
}
template<class T> std::istream& operator>> (std::istream &in,Polynomial<T> &a)
{
    a=Polynomial<T>();
#ifdef INTELLIGENT_H
    if(INTELLIGENT)
    {
        std::string s;
        getline(in,s);
        if(!s.empty())
            a=Polynomial<T>(s);
        return in;
    }
#endif
    int n,exp;
    T fac;
    in>>n;
    while(n--)
    {
        in>>fac>>exp;
        a.pol.emplace_back(exp,fac);
    }
    a.simplify();
    return in;
}
template<class T> std::ostream& operator<< (std::ostream &out,const Polynomial<T> &a)
{
    if(!a.pol.size())
        out<<"0";
    else
        for(int i=0;i<a.pol.size();i++)
        {
            if(a.pol[i].second>T(0)&&i)
                out<<"+";
            if(a.pol[i].first==0)
                out<<a.pol[i].second;
            else
            {
                if(a.pol[i].second<T(0)&&a.pol[i].second==T(-1))
                    out<<"-";
                else if(a.pol[i].second!=T(1))
                    out<<a.pol[i].second;
                out<<"x";
                if(a.pol[i].first!=1)
                    out<<"^"<<a.pol[i].first;
            }
        }
    return out;
}

template<class T> Polynomial<T> gcd(Polynomial<T> a,Polynomial<T> b)
{
    T g1=T(0),g2=T(0);
    for(int i=0;i<a.pol.size();i++)
        g1=gcd(g1,a.pol[i].second);
    for(int i=0;i<b.pol.size();i++)
        g2=gcd(g2,b.pol[i].second);
    a/=g1,b/=g2;
    while(!a.pol.empty()&&!b.pol.empty())
    {
        if(a.pol.back().first<b.pol.back().first)
            std::swap(a,b);
        T L=lcm(a.pol.back().second,b.pol.back().second);
        a*=(L/a.pol.back().second);
        b*=(L/b.pol.back().second);
        Polynomial<T> c;
        if(a.pol.back().second==b.pol.back().second)
            c.pol.emplace_back(a.pol.back().first-b.pol.back().first,-1);
        else
            c.pol.emplace_back(a.pol.back().first-b.pol.back().first,1);
        a+=b*c;
    }
    a+=b;
    T g3=T(0);
    for(int i=0;i<a.pol.size();i++)
        g3=gcd(g3,a.pol[i].second);
    a*=gcd(g1,g2);a/=g3;
    if(a.pol.back().second<T(0))
        a=-a;
    return a;
}
template<class T> Polynomial<T> lcm(Polynomial<T> a,Polynomial<T> b){return a*b/gcd(a,b);}

/*
 *Hello!
 *The class can store polynomial whose factor can be almost any type!
 *It provides an intelligent reading that can exchange an expression(surely string) into a polynomial.
 *However,there are some restraints:
 *1.The exponent must be a natural number and can be beyond the range of int.
 *2.The input expression must be legal.More precisely you can only use the following elements:
 *  (1)Number of your type(temporarily only support integer type).
 *  (2)Arithmetic operator(+,-,* and ^).
 *  (3)Parentheses(should be matchable).
 *  (4)Variables(only 'x').
 *3.No space can be contained in the expression.
 *4.Multiplication signs can be omitted only in the following situation:
 *  (1)between two parentheses.
 *  (2)between number(factor) and 'x'.
 *  (3)between parenthesis and number(should be in the front).
 *  (4)between parenthesis and 'x'.
 *There are some examples:
 *  Legal:5x^3+2x^2-(x+12)^2*50 ; 5x*x*x^5+(5x*x*x)^5+(-5+2)^2 ; x(-x+1)(x+2)x ; 5+(-x)
 *  Illegal:5x3 ; (x+5)50 ; x^50x ; xx ; 5+-x
 */
template<class T>
class Polynomial
{
    friend Polynomial operator+ <T> (const Polynomial&,const Polynomial&);
    friend Polynomial operator+ <T> (const Polynomial&);
    friend Polynomial operator- <T> (const Polynomial&);
    friend Polynomial operator- <T> (const Polynomial&,const Polynomial&);
    friend Polynomial operator* <T> (const Polynomial&,const Polynomial&);
    friend Polynomial operator* <T> (const Polynomial&,const T&);
    friend Polynomial operator* <T> (const T&,const Polynomial&);
    friend Polynomial operator/ <T> (const Polynomial&,const T&);
    friend Polynomial operator/ <T> (const Polynomial&,const Polynomial&);
    friend Polynomial operator% <T> (const Polynomial&,const T&);
    friend Polynomial operator% <T> (const Polynomial&,const Polynomial&);
    friend Polynomial operator^ <T> (Polynomial,int);
    friend std::istream& operator>> <T> (std::istream&,Polynomial&);
    friend std::ostream& operator<< <T> (std::ostream&,const Polynomial&);
    friend Polynomial gcd <T> (Polynomial,Polynomial);
private:
    std::vector<std::pair<int,T>> pol;
    void simplify();
public:
    Polynomial():pol(std::vector<std::pair<int,T>>()){}
    explicit Polynomial(const T &a){pol.clear();if(a)pol.emplace_back(0,a);}
#ifdef INTELLIGENT_H
    explicit Polynomial(std::string s);
#endif
    T f(const T &x);
    Polynomial& operator=(const Polynomial &a);
    Polynomial& operator+= (const Polynomial &a);
    Polynomial& operator-= (const Polynomial &a);
    Polynomial& operator*= (const T &a);
    Polynomial& operator*= (const Polynomial &a);
    Polynomial& operator/= (const T &a);
    Polynomial& operator/= (const Polynomial &a);
    Polynomial& operator%= (const T &a);
    Polynomial& operator%= (const Polynomial &a);
    Polynomial& operator^= (const int &a);
    explicit operator bool(){return !pol.empty();}
};

template<class T> void Polynomial<T>::simplify()
{
    int chk=1;
    for(int i=1;i<pol.size();i++)
        if(pol[i-1].first>pol[i].first)
            chk=0;
    if(!chk)
        std::sort(pol.begin(),pol.end());
    for(int i=0;i<pol.size();i++)
    {
        if(!pol[i].second)
            pol[i].first=INT_MAX;
        else if(i&&pol[i].first==pol[i-1].first)
            pol[i-1].first=INT_MAX,pol[i].second+=pol[i-1].second;
    }
    int pos=0;
    for(int i=0;i<pol.size();i++)
        if(pol[i].first<INT_MAX)
            pol[pos++]=pol[i];
    while(pol.size()!=pos)
        pol.pop_back();
}

#ifdef INTELLIGENT_H
template<class T> Polynomial<T>::Polynomial(std::string s)
{
    pol.clear();
    std::stack<char> sta;
    std::string s0,t;
    for(int i=0;i<s.size();i++)
    {
        if(s[i]=='-'&&(!i||s[i-1]=='('||is_symbol(s[i-1])))
        {
            if(i&&s0.back()=='~')
                s0.pop_back();
            else
                s0+='~';
        }
        else if(s[i]=='+'&&(!i||s[i-1]=='('||is_symbol(s[i-1]))){}
        else if((s[i]>='0'&&s[i]<='9')||s[i]==')'||s[i]=='+'||s[i]=='-'||s[i]=='*'||s[i]=='/'||s[i]=='%'||s[i]=='^'||s[i]=='.')
            s0+=s[i];
        else if(s[i]=='('||s[i]=='x')
        {
            if(i&&(s[i-1]==')'||s[i-1]=='x'||(s[i-1]>='0'&&s[i-1]<='9')))
                s0+='*';
            s0+=s[i];
        }
        else if(s[i]!=' '&&s[i]!='\t')
            throw invalid_expression("Illegal symbol!");
    }
    char las=0,las0=0;
    for(auto a:s0)
    {
        if(a==')'&&(las=='+'||las=='-'||las=='*'||las=='/'||las=='%'||las=='^'||las=='~'||las=='#'||las=='('))
            throw invalid_expression("Invalid expression!");
        if((a=='.'||(a>='0'&&a<='9'))&&(las=='x'||las==')'))
            throw invalid_expression("Invalid expression!");
        if(las=='.'&&(a<'0'||a>'9')&&(las0<'0'||las0>'9'))
            throw invalid_expression("Invalid expression!");
        if((a>='0'&&a<='9')||a=='.')
        {
            if((las<'0'||las>'9')&&las!='.')
                t+='|';
            t+=a;
        }
        else if(a=='x')
            t+=a;
        else if(a=='(')
            sta.push(a);
        else if(a==')')
        {
            while(!sta.empty()&&sta.top()!='(')
                t+=sta.top(),sta.pop();
            if(sta.empty())
                throw invalid_expression("Invalid expression!");
            sta.pop();
        }
        else if(a=='+'||a=='-')
        {
            while(!sta.empty()&&(sta.top()=='+'||sta.top()=='-'||sta.top()=='~'||sta.top()=='#'||sta.top()=='*'||sta.top()=='/'||sta.top()=='%'||sta.top()=='^'))
                t+=sta.top(),sta.pop();
            sta.push(a);
        }
        else if(a=='*'||a=='/'||a=='%')
        {
            while(!sta.empty()&&(sta.top()=='*'||sta.top()=='/'||sta.top()=='%'||sta.top()=='^'||sta.top()=='~'||sta.top()=='#'))
                t+=sta.top(),sta.pop();
            sta.push(a);
        }
        else if(a=='~'||a=='#')
        {
            while(!sta.empty()&&(sta.top()=='^'||sta.top()=='~'||sta.top()=='#'))
                t+=sta.top(),sta.pop();
            sta.push(a);
        }
        else
        {
            while(!sta.empty()&&sta.top()=='^')
                t+=sta.top(),sta.pop();
            sta.push(a);
        }
        las0=las;las=a;
    }
    if(las=='.'&&(las0<'0'||las0>'9'))
        throw invalid_expression("Invalid expression!");
    while(!sta.empty())
    {
        if(sta.top()=='(')
            throw invalid_expression("Invalid expression!");
        t+=sta.top(),sta.pop();
    }
    std::stack<Polynomial > sta0;
    Polynomial x;
    x.pol.emplace_back(1,T(1));
    int pos=0;
    while(pos!=t.size())
    {
        if(t[pos]=='x')
            sta0.push(x);
        else if(t[pos]=='|')
        {
            T a(0);
            while(pos!=t.size()-1&&t[pos+1]>='0'&&t[pos+1]<='9')
                a=a*T(10)+T(t[++pos]-'0');
            if(pos!=t.size()-1&&t[pos+1]=='.')
            {
                pos++;
                T pw=T(1);
                while(pos!=t.size()-1&&t[pos+1]>='0'&&t[pos+1]<='9')
                    pw/=T(10),a+=T(t[++pos]-'0')*pw;
                if(pos!=t.size()-1&&t[pos+1]=='.')
                    throw invalid_expression("Invalid expression!");
            }
            sta0.push(Polynomial(a));
        }
        else if(t[pos]=='~')
        {
            if(sta0.empty())
                throw invalid_expression("Invalid expression!");
            const auto a=sta0.top();sta0.pop();sta0.push(-a);
        }
        else if(t[pos]=='#')
        {
            if(sta0.empty())
                throw invalid_expression("Invalid expression!");
        }
        else if(t[pos]=='+')
        {
            if(sta0.size()<2)
                throw invalid_expression("Invalid expression!");
            const auto b=sta0.top();sta0.pop();
            const auto a=sta0.top();sta0.pop();
            sta0.push(a+b);
        }
        else if(t[pos]=='-')
        {
            if(sta0.size()<2)
                throw invalid_expression("Invalid expression!");
            const auto b=sta0.top();sta0.pop();
            const auto a=sta0.top();sta0.pop();
            sta0.push(a-b);
        }
        else if(t[pos]=='*')
        {
            if(sta0.size()<2)
                throw invalid_expression("Invalid expression!");
            const auto b=sta0.top();sta0.pop();
            const auto a=sta0.top();sta0.pop();
            sta0.push(a*b);
        }
        else if(t[pos]=='/')
        {
            if(sta0.size()<2)
                throw invalid_expression("Invalid expression!");
            const auto b=sta0.top();sta0.pop();
            const auto a=sta0.top();sta0.pop();
            sta0.push(a/b);
        }
        else if(t[pos]=='%')
        {
            if(sta0.size()<2)
                throw invalid_expression("Invalid expression!");
            const auto b=sta0.top();sta0.pop();
            const auto a=sta0.top();sta0.pop();
            sta0.push(a%b);
        }
        else if(t[pos]=='^')
        {
            if(sta0.size()<2)
                throw invalid_expression("Invalid expression!");
            const auto b=sta0.top();sta0.pop();
            const auto a=sta0.top();sta0.pop();
            if(b.pol.size()>1||(b.pol.size()==1&&b.pol[0].first!=0))
                throw invalid_expression("Invalid expression!");
            int b0=b.pol.empty()?0:static_cast<int>(b.pol[0].second);
            if(b0<0)
                throw invalid_expression("Invalid expression!");
            sta0.push(a^b0);
        }
        pos++;
    }
    if(sta0.size()!=1)
        throw invalid_expression("Invalid expression!");
    *this=sta0.top();
}
#endif

template<class T> T Polynomial<T>::f(const T &x)
{
    T ans=T(0),pw=T(1);
    int las=0;
    for(auto [exp,fac]:pol)
    {
        int y=exp-las;T lazy=x;
        while(y)
        {
            if(y&1)
                pw*=lazy;
            lazy*=lazy;
            y>>=1;
        }
        ans+=pw*fac,las=exp;
    }
    return ans;
}

template<class T> Polynomial<T>& Polynomial<T>::operator=(const Polynomial &a)=default;
template<class T> Polynomial<T>& Polynomial<T>::operator+=(const Polynomial &a){return *this=*this+a;}
template<class T> Polynomial<T>& Polynomial<T>::operator-=(const Polynomial &a){return *this=*this-a;}
template<class T> Polynomial<T>& Polynomial<T>::operator*=(const T &a){return *this=*this*a;}
template<class T> Polynomial<T>& Polynomial<T>::operator*=(const Polynomial &a){return *this=*this*a;}
template<class T> Polynomial<T>& Polynomial<T>::operator/=(const T &a){return *this=*this/a;}
template<class T> Polynomial<T>& Polynomial<T>::operator/=(const Polynomial &a){return *this=*this/a;}
template<class T> Polynomial<T>& Polynomial<T>::operator%=(const T &a){return *this=*this%a;}
template<class T> Polynomial<T>& Polynomial<T>::operator%=(const Polynomial &a){return *this=*this%a;}
template<class T> Polynomial<T>& Polynomial<T>::operator^=(const int &a){return *this=*this^a;}

#endif