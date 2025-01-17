#ifndef COMPLEX_H
#define COMPLEX_H

#include<iostream>
#include<cmath>

#include "intelligent.h"

template<class T,int base=-1> class Complex;
template<class T,int base=-1> Complex<T,base> operator+ (const Complex<T,base> &a,const Complex<T,base> &b)
{
    return Complex<T,base>(a.real+b.real,a.imag+b.imag);
}
template<class T,int base=-1> Complex<T,base> operator- (const Complex<T,base> &a,const Complex<T,base> &b)
{
    return Complex<T,base>(a.real-b.real,a.imag-b.imag);
}
template<class T,int base=-1> Complex<T,base> operator* (const Complex<T,base> &a,const Complex<T,base> &b)
{
    return Complex<T,base>(a.real*b.real+T(base)*a.imag*b.imag,a.real*b.imag+a.imag*b.real);
}
template<class T,int base=-1> T abs2(const Complex<T,base> &a){return (a*a.con()).real;}
template<class T,int base=-1> T abs(const Complex<T,base> &a){return sqrt(abs2(a));}
template<class T,int base=-1> Complex<T,base> operator/ (const Complex<T,base> &a,const Complex<T,base> &b)
{
    if(!static_cast<bool>(b))
        throw std::runtime_error("The divisor can't be zero!");
    else
    {
        Complex<T,base> tmp=a*b.con();
        T div=abs2(b);
        return Complex<T,base>(tmp.real/T(div),tmp.imag/T(div));
    }
}
template<class T,int base=-1> Complex<T,base> operator- (const Complex<T,base> &a)
{
    return Complex<T,base>(-a.real,-a.imag);
}
template<class T,int base=-1> std::ostream& operator<< (std::ostream &out,const Complex<T,base> &a)
{
#ifdef INTELLIGENT_H
    if(!INTELLIGENT)
#endif
        out<<"("<<a.real<<","<<a.imag<<")";
#ifdef INTELLIGENT_H
    else
    {
        if(!static_cast<bool>(a))
            out<<0;
        else if(a.real)
        {
            out<<a.real;
            if(a.imag>T(0))
            {
                if(a.imag!=T(1))
                    out<<"+"<<a.imag<<"i";
                else
                    out<<"+i";
            }
            else if(a.imag<T(0))
            {
                if(a.imag!=T(-1))
                    out<<"-"<<-a.imag<<"i";
                else
                    out<<"-i";
            }
        }
        else if(!a.imag)
            out<<a.real;
        else
        {
            if(a.imag==T(1))
                out<<"i";
            else if(a.imag==T(-1))
                out<<"-i";
            else if(a.imag>T(0))
                out<<a.imag<<"i";
            else
                out<<"-"<<-a.imag<<"i";
        }
    }
#endif
    return out;
}
template<class T,int base=-1> std::istream& operator>> (std::istream &in,Complex<T,base> &a)
{
    in>>a.real>>a.imag;
    return in;
}
template<class T,int base=-1> bool operator== (const Complex<T,base> &a,const Complex<T,base> &b)
{
    return a.real==b.real&&a.imag==b.imag;
}

template<class T,int base>
class Complex
{
    friend Complex operator+ <T,base> (const Complex&,const Complex&);
    friend Complex operator- <T,base> (const Complex&,const Complex&);
    friend Complex operator* <T,base> (const Complex&,const Complex&);
    friend Complex operator/ <T,base> (const Complex&,const Complex&);
    friend Complex operator- <T,base> (const Complex&);
    friend T abs2 <T,base> (const Complex&);
    friend T abs <T,base> (const Complex&);
    friend std::ostream& operator<< <T,base> (std::ostream&,const Complex &);
    friend std::istream& operator>> <T,base> (std::istream&,Complex &);
    friend bool operator== <T,base> (const Complex&,const Complex&);
private:
    T real,imag;
public:
    Complex():real(0),imag(0){}
    explicit Complex(const T& a):real(a),imag(0){}
    Complex(const T& a,const T& b):real(a),imag(b){}
    T& re(){return real;}
    T& im(){return imag;}
    Complex con()const{return Complex(real,-imag);}
    Complex& operator+= (const Complex &a);
    Complex& operator-= (const Complex &a);
    Complex& operator*= (const Complex &a);
    Complex& operator/= (const Complex &a);
    explicit operator bool()const;
};

template<class T,int base>Complex<T,base>& Complex<T,base>::operator+=(const Complex<T,base> &a){return *this=*this+a;}
template<class T,int base>Complex<T,base>& Complex<T,base>::operator-=(const Complex<T,base> &a){return *this=*this-a;}
template<class T,int base>Complex<T,base>& Complex<T,base>::operator*=(const Complex<T,base> &a){return *this=*this*a;}
template<class T,int base>Complex<T,base>& Complex<T,base>::operator/=(const Complex<T,base> &a){return *this=*this/a;}
template<class T,int base>Complex<T,base>::operator bool()const{return real||imag;}

#endif
