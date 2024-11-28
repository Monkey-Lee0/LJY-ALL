#ifndef FRACTION_H
#define FRACTION_H

using std::gcd;

#include<numeric>

template<class T>class Fraction;
template<class T> Fraction<T> operator+ (const Fraction<T> &a,const Fraction<T> &b)
{
	return Fraction<T>(a.num*b.den+a.den*b.num,a.den*b.den);
}
template<class T> Fraction<T> operator* (const Fraction<T> &a,const Fraction<T> &b)
{
	return Fraction<T>(a.num*b.num,a.den*b.den);
}
template<class T> Fraction<T> operator/ (const Fraction<T> &a,const Fraction<T> &b)
{
	if(!b.num)
		throw std::runtime_error("The divisor can't be zero!");
	return Fraction<T>(a.num*b.den,a.den*b.num);
}
template<class T> Fraction<T> operator- (const Fraction<T> &a)
{
	return Fraction<T>(-a.num,a.den);
}
template<class T> Fraction<T> operator- (const Fraction<T> &a,const Fraction<T> &b)
{
	return a+(-b);
}
template<class T> std::ostream& operator<< (std::ostream &out,const Fraction<T> &a)
{
	out<<a.num;
	if(a.den!=T(1))
		out<<"/"<<a.den;
	return out;
}
template<class T> std::istream& operator>> (std::istream &in,Fraction<T> &a)
{
	in>>a.num>>a.den;
	a.Reduce();
	return in;
}
template<class T> bool operator== (const Fraction<T> &a,const Fraction<T> &b)
{
	return a.num==b.num&&a.den==b.den;
}
template<class T> std::strong_ordering operator<=> (const Fraction<T> &a,const Fraction<T> &b)
{
	T val1=a.num*b.den,val2=b.num*a.den;
	if(val1<val2)
		return std::strong_ordering::less;
	if(val1==val2)
		return std::strong_ordering::equal;
	return std::strong_ordering::greater;
}
template<class T> Fraction<T> abs(const Fraction<T>& a){return Fraction<T>(abs(a.num),a.den);}

template<class T=int>
class Fraction
{
	friend Fraction operator+ <T> (const Fraction&,const Fraction&);
	friend Fraction operator- <T> (const Fraction&,const Fraction&);
	friend Fraction operator* <T> (const Fraction&,const Fraction&);
	friend Fraction operator/ <T> (const Fraction&,const Fraction&);
	friend Fraction operator- <T> (const Fraction<T>&);
	friend std::ostream& operator<< <T> (std::ostream&,const Fraction&);
	friend std::istream& operator>> <T> (std::istream&,Fraction&);
	friend bool operator== <T> (const Fraction&,const Fraction&);
	friend std::strong_ordering operator<=> <T> (const Fraction&,const Fraction&);
	friend Fraction abs<T> (const Fraction&);
private:
	T num,den;
	void Reduce()
	{
		const T g=gcd(num,den);
		num/=g,den/=g;
		if(den<T(0))
			num=-num,den=-den;
	}
public:
	Fraction():num(T(0)),den(T(1)){}
	explicit Fraction(const T &a):num(a),den(T(1)){}
	Fraction (const T &a,const T &b):num(a),den(b)
	{
		if(!static_cast<bool>(b))
			throw std::runtime_error("The denominator can't be zero!");
		else
			Reduce();
	}
	Fraction& operator+= (const Fraction &a);
	Fraction& operator-= (const Fraction &a);
	Fraction& operator*= (const Fraction &a);
	Fraction& operator/= (const Fraction &a);
	Fraction& operator++ ();
	Fraction operator++ (int tmp);
	Fraction& operator-- ();
	Fraction operator-- (int tmp);
	explicit operator double()const;
	explicit operator long double()const;
	explicit operator bool()const;
};

template<class T> Fraction<T>& Fraction<T>::operator+= (const Fraction<T> &a){return *this=*this+a;}
template<class T> Fraction<T>& Fraction<T>::operator-= (const Fraction<T> &a){return *this=*this-a;}
template<class T> Fraction<T>& Fraction<T>::operator*= (const Fraction<T> &a){return *this=*this*a;}
template<class T> Fraction<T>& Fraction<T>::operator/= (const Fraction<T> &a){return *this=*this/a;}
template<class T> Fraction<T>& Fraction<T>::operator++ (){(*this)+=Fraction<T>(1);return *this;}
template<class T> Fraction<T> Fraction<T>::operator++ (int){Fraction<T> res=*this;(*this)+=Fraction<T>(1);return res;}
template<class T> Fraction<T>& Fraction<T>::operator-- (){(*this)-=Fraction<T>(1);return *this;}
template<class T> Fraction<T> Fraction<T>::operator-- (int){Fraction<T> res=*this;(*this)-=Fraction<T>(1);return res;}
template<class T> Fraction<T>::operator double()const{return 1.0*num/den;}
template<class T> Fraction<T>::operator long double()const{return 1.0l*num/den;}
template<class T> Fraction<T>::operator bool()const{return (num!=0);}

#endif
