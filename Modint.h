#ifndef MODINT_H
#define MODINT_H

#include<iostream>

extern const int MOD;//If you want a default modulus,you can define it in your main.cpp.

template<int mod=MOD>class modint;
template<int mod> modint<mod> mod_maintain(const modint<mod> &a){return modint<mod>(a.x>=mod?a.x-mod:a.x);}
template<int mod> modint<mod> operator+ (const modint<mod> &a,const modint<mod> &b){return mod_maintain(modint<mod>(a.x+b.x));}
template<int mod> modint<mod> operator- (const modint<mod> &a,const modint<mod> &b){return mod_maintain(modint<mod>(a.x-b.x+mod));}
template<int mod> modint<mod> operator* (const modint<mod> &a,const modint<mod> &b){return modint<mod>(1ll*a.x*b.x%mod);}
template<int mod> modint<mod> power(modint<mod> a,int k=mod-2)
{
    modint<mod> ans(1);
    while(k)
    {
        if(k&1)
            ans=ans*a;
        a=a*a;
        k>>=1;
    }
    return ans;
}
template<int mod> bool operator==(const modint<mod> &a,const modint<mod> &b){return a.x==b.x;}
template<int mod> bool operator==(const modint<mod> &a,const int &b){return (a.x-b)%mod==0;}
template<int mod> modint<mod> operator/ (const modint<mod> &a,const modint<mod> &b)
{
    if(b.x==0)
        throw std::runtime_error("The divisor can't be zero!");
    modint<mod> inv=power(b);
    if(b*inv!=modint<mod>(1))
        throw std::runtime_error("The division is undefined!");
    return inv*a;
}
template<int mod> modint<mod> operator- (const modint<mod> &a){return mod_maintain(modint<mod>(mod-a.x));}
template<int mod> std::istream& operator>> (std::istream &in,modint<mod> &a){in>>a.x;a.x=(a.x>=0?a.x%mod:a.x%mod+mod);return in;}
template<int mod> std::ostream& operator<< (std::ostream &out,const modint<mod> &a){out<<a.x;return out;}
template<int mod> std::strong_ordering operator<=> (const modint<mod> &a,const modint<mod> &b)
{
    if(a.x<b.x)
        return std::strong_ordering::less;
    if(a.x==b.x)
        return std::strong_ordering::equal;
    return std::strong_ordering::greater;
}

template<int mod>
class modint
{
    friend modint mod_maintain<mod>(const modint&);
    friend modint operator+ <mod> (const modint&,const modint&);
    friend modint operator- <mod> (const modint&,const modint&);
    friend modint operator* <mod> (const modint&,const modint&);
    friend modint power<mod>(modint,int);
    friend bool operator== <mod> (const modint&,const modint&);
    friend bool operator== <mod> (const modint&,const int&);
    friend modint operator/ <mod> (const modint&,const modint&);//Please DO NOT use it when mod is not a prime!
    friend modint operator- <mod> (const modint&);
    friend std::istream& operator>> <mod> (std::istream&,modint&);
    friend std::ostream& operator<< <mod> (std::ostream&,const modint&);
    friend std::strong_ordering operator<=> <mod> (const modint&,const modint&);
private:
    int x;
public:
    modint():x(0){}
    explicit modint(const int& a,const bool& op=true):x(a){op&&((x=(x%mod+mod)%mod));}
    modint& operator=(const int &a);
    modint& operator+= (const modint &a);
    modint& operator-= (const modint &a);
    modint& operator*= (const modint &a);
    modint& operator/= (const modint &a);
    modint& operator++ ();
    modint operator++ (int tmp);
    modint& operator-- ();
    modint operator-- (int tmp);
	explicit operator int()const;
	explicit operator bool()const;
};

template<int mod> modint<mod>& modint<mod>::operator=(const int &a)
{
    *this=modint(a,true);
    return *this;
}
template<int mod> modint<mod>& modint<mod>::operator+= (const modint &a){return *this=*this+a;}
template<int mod> modint<mod>& modint<mod>::operator-= (const modint &a){return *this=*this-a;}
template<int mod> modint<mod>& modint<mod>::operator*= (const modint &a){return *this=*this*a;}
template<int mod> modint<mod>& modint<mod>::operator/= (const modint &a){return *this=*this/a;}
template<int mod> modint<mod>& modint<mod>::operator++ (){(*this)+=modint(1);return *this;}
template<int mod> modint<mod> modint<mod>::operator++ (int){modint res=*this;(*this)+=modint(1);return res;}
template<int mod> modint<mod>& modint<mod>::operator-- (){(*this)-=modint(1);return *this;}
template<int mod> modint<mod> modint<mod>::operator-- (int){modint res=*this;(*this)-=modint(1);return res;}
template<int mod> modint<mod>::operator int()const{return x;}
template<int mod> modint<mod>::operator bool()const{return x>0;}

#endif