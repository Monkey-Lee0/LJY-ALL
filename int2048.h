#ifndef INT2048_H
#define INT2048_H

#include<cstring>
#include<vector>
#include<iomanip>
#include<climits>
#include<complex>

class int2048;
class int65536;

/*
*It is,however,a trivial edition,which can store little digits.
*But it is faster than int65536 when dealing with small(relatively) number.
*/

constexpr int M_PW10[]={1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};

constexpr int MAX_LENGTH=240;
constexpr int MAX_NUM=1e9;
constexpr int MAX_DIGIT=9;

class int2048// This class can store integral values only between -(10^2160-1)~(10^2160-1).It depends on the value of MAX_LENGTH.
{
	friend class int65536;
	friend int2048 operator+ (const int2048&);
	friend int2048 operator+ (const int2048&,const int2048&);
	friend int2048 operator- (const int2048&);
	friend int2048 operator- (const int2048&,const int2048&);
	friend int2048 operator* (const int2048&,const int&);
	friend int2048 operator* (const int&,const int2048&);
	friend int2048 operator* (const int2048&,const int2048&);
	friend int2048 operator/ (const int2048&,const int&);
	friend int2048 operator/ (const int2048&,const int2048&);
	friend int2048 operator% (const int2048&,const int&);
	friend int2048 operator% (const int2048&,const int2048&);
	friend int2048 operator^ (const int2048&,const int2048&);
	friend std::istream& operator>> (std::istream&,int2048&);
	friend std::ostream& operator<< (std::ostream&,const int2048&);
	friend bool operator== (const int2048&,const int2048&);
	friend std::strong_ordering operator<=> (const int2048&,const int2048&);
	friend int2048 gcd(int2048 a,int2048 b);
private:
	int Max_length=0;
	bool symbol=true;// True represents plus while false represents minus.
	int num[MAX_LENGTH]{};
	void simplify(){while(Max_length&&!num[Max_length-1])Max_length--;}
public:
	int2048(){memset(num,0,sizeof(num));}
	explicit int2048(const int &);
	explicit int2048(const long long &);
	explicit int2048(const std::string &);
	int2048(const int2048 &);
	explicit int2048(const int65536 &);
	[[nodiscard]] int at(const int &a) const;
	[[nodiscard]] int parity()const{return num[0]&1;}
	int2048& operator=(const int2048&);
	int2048& operator+=(const int2048&);
	int2048& operator-=(const int2048&);
	int2048& operator*=(const int&);
	int2048& operator*=(const int2048&);
	int2048& operator/=(const int&);
	int2048& operator/=(const int2048&);
	int2048& operator%=(const int&);
	int2048& operator%=(const int2048&);
	int2048& operator^=(const int2048&);
	int2048& operator++ ();
	int2048 operator++ (int tmp);
	int2048& operator-- ();
	int2048 operator-- (int tmp);
	explicit operator bool()const{return Max_length>0;}
	explicit operator int()const;
	explicit operator long long()const;
	explicit operator std::string()const;
};

inline int2048::int2048(const int &a):Max_length(1)
{
	memset(num,0,sizeof(num));
	num[0]=abs(a);
	if(a<0)
		symbol=false;
	if(num[0]>=MAX_NUM)
		num[++Max_length]+=num[0]/MAX_NUM,num[0]-=num[1]*MAX_NUM;
}
inline int2048::int2048(const long long &a)
{
	unsigned long long tmp=a;
	if(a<0)
		symbol=false,tmp=-tmp;
	num[Max_length++]=(static_cast<int>(tmp%MAX_NUM));
	if(tmp>=MAX_NUM)
	{
		tmp/=MAX_NUM;
		num[Max_length++]=static_cast<int>(tmp%MAX_NUM);
		if(tmp>=MAX_NUM)
			num[Max_length++]=static_cast<int>(tmp/MAX_NUM);
	}
	simplify();
}
inline int2048::int2048(const std::string &s)
{
	for(int i=0;i<static_cast<int>(s.size());i++)
	{
		if(s[i]!='-'&&s[i]!='+'&&(s[i]<'0'||s[i]>'9'))
		{
#ifdef INTELLIGENT_H
			throw invalid_expression("Illegal symbol!");
#endif
			return ;
		}
		if(s[i]=='-'&&i)
		{
#ifdef INTELLIGENT_H
			throw invalid_expression("Illegal symbol!");
#endif
			return ;
		}
		if(s[i]=='+'&&i)
		{
#ifdef INTELLIGENT_H
			throw invalid_expression("Illegal symbol!");
#endif
			return ;
		}
	}
	Max_length=static_cast<int>(s.size())/9+1;
	int l=0,r=static_cast<int>(s.size())%MAX_DIGIT;
	if(!s.empty()&&s[0]=='-')
		symbol=false,l++;
	if(!s.empty()&&s[0]=='+')
		l++;
	for(int pos=Max_length-1;pos>=0;pos--)
	{
		for(int i=l;i<r;i++)
			num[pos]=num[pos]*10+s[i]-'0';
		l=r,r=l+MAX_DIGIT;
	}
	simplify();
}
inline int2048::int2048(const int2048 &a)
{
	Max_length=a.Max_length;
	symbol=a.symbol;
	for(int i=0;i<Max_length;i++)
		num[i]=a.num[i];
}
inline int int2048::at(const int &a)const
{
	if(a<=0||a>Max_length*MAX_DIGIT)
		return 0;
	const int pos=(a-1)/MAX_DIGIT,res=a-pos*MAX_DIGIT;
	return num[pos]/M_PW10[res-1]%10;
}

inline int2048& int2048::operator=(const int2048 &a)
{
	if(this==&a)
		return *this;
	for(int i=0;i<Max_length;i++)
		num[i]=0;
	Max_length=a.Max_length;
	symbol=a.symbol;
	for(int i=0;i<Max_length;i++)
		num[i]=a.num[i];
	return *this;
}
inline int2048& int2048::operator+=(const int2048 &a){return *this=*this+a;}
inline int2048& int2048::operator-=(const int2048 &a){return *this=*this-a;}
inline int2048& int2048::operator*=(const int &a){return *this=*this*a;}
inline int2048& int2048::operator*=(const int2048 &a){return *this=*this*a;}
inline int2048& int2048::operator/=(const int &a){return *this=*this/a;}
inline int2048& int2048::operator/=(const int2048 &a){return *this=*this/a;}
inline int2048& int2048::operator%=(const int &a){return *this=*this%a;}
inline int2048& int2048::operator%=(const int2048 &a){return *this=*this%a;}
inline int2048& int2048::operator^=(const int2048 &a){return *this=*this^a;}
inline int2048& int2048::operator++(){return *this=*this+int2048(1);}
inline int2048 int2048::operator++(int tmp){int2048 a=*this;*this=*this+int2048(1);return a;}
inline int2048& int2048::operator--(){return *this=*this-int2048(1);}
inline int2048 int2048::operator--(int tmp){int2048 a=*this;*this=*this-int2048(1);return a;}

inline int2048 operator+ (const int2048 &a){return a;}
inline int2048 operator+ (const int2048 &a,const int2048 &b)
{
	int2048 c;c.Max_length=std::max(a.Max_length,b.Max_length);c.symbol=a.symbol;
	if(a.symbol==b.symbol)
	{
		for(int i=0;i<c.Max_length;i++)
			c.num[i]=a.num[i]+b.num[i];
		for(int i=0;i<c.Max_length;i++)
			if(c.num[i]>=MAX_NUM)
				c.num[i]-=MAX_NUM,c.num[i+1]++;
		if(c.num[c.Max_length])
			c.Max_length++;
	}
	else
	{
		for(int i=0;i<c.Max_length;i++)
			c.num[i]=a.num[i]-b.num[i];
		for(int i=0;i<c.Max_length;i++)
			if(c.num[i]<0)
				c.num[i]+=MAX_NUM,c.num[i+1]--;
		if(c.num[c.Max_length])
			c.Max_length++;
		else
			c.simplify();
		if(c.Max_length&&c.num[c.Max_length-1]<0)
		{
			c.symbol^=1;
			for(int i=c.Max_length-1;i>=0;i--)
				c.num[i]=-c.num[i];
			for(int i=0;i<c.Max_length;i++)
				if(c.num[i]<0)
					c.num[i+1]--,c.num[i]+=MAX_NUM;
			c.simplify();
		}
	}
	return c;
}
inline int2048 operator- (const int2048 &a)
{
	int2048 b(a);
	b.symbol^=1;
	return b;
}
inline int2048 operator- (const int2048 &a,const int2048 &b){return a+(-b);}
inline int2048 operator* (const int2048 &a,const int &b)
{
	long long carry=0;
	int2048 c;
	c.Max_length=a.Max_length;
	c.symbol=a.symbol;
	long long t=b;
	if(b<0)
		t=-t,c.symbol^=1;
	for(int i=0;i<a.Max_length;i++)
	{
		const long long tmp=1ll*a.num[i]*t+carry;
		carry=static_cast<int>(tmp/MAX_NUM);
		c.num[i]=static_cast<int>(tmp-1ll*carry*MAX_NUM);
	}
	while(carry)
	{
		const long long tmp=carry/MAX_NUM;
		c.num[c.Max_length++]=static_cast<int>(carry-tmp*MAX_NUM);
		carry=tmp;
	}
	c.simplify();
	return c;
}
inline int2048 operator* (const int &a,const int2048 &b){return b*a;}
inline int2048 operator* (const int2048 &a,const int2048 &b)
{
	int2048 c;
	c.symbol=a.symbol^b.symbol^1;
	c.Max_length=a.Max_length+b.Max_length;
	for(int i=0;i<a.Max_length;i++)
	{
		const int2048 tmp=b*a.num[i];
		for(int j=0;j<tmp.Max_length;j++)
			c.num[i+j]=c.num[i+j]+tmp.num[j];
		for(int j=i;j<tmp.Max_length+i;j++)
			if(c.num[j]>=MAX_NUM)
				c.num[j]-=MAX_NUM,c.num[j+1]++;
	}
	for(int i=0;i<c.Max_length;i++)
		if(c.num[i]>=MAX_NUM)
			c.num[i]-=MAX_NUM,c.num[i+1]++;
	c.simplify();
	return c;
}
inline int2048 operator/ (const int2048 &a,const int &b)
{
	if(!b)
		throw std::runtime_error("The divisor can't be zero!");
	int2048 c;
	c.symbol=a.symbol;
	c.Max_length=a.Max_length;
	long long tmp=b;
	if(b<0)
		tmp=-tmp,c.symbol^=1;
	int carry=0;
	for(int i=c.Max_length-1;i>=0;i--)
	{
		const long long now=1ll*carry*MAX_NUM+a.num[i];
		c.num[i]=static_cast<int>(now/tmp);
		carry=static_cast<int>(now-1ll*c.num[i]*tmp);
	}
	c.simplify();
	return c;
}
inline int2048 operator/ (const int2048 &a,const int2048 &b)
{
	if(!b.Max_length)
		throw std::runtime_error("The divisor can't be zero!");
	if(a.Max_length<b.Max_length)
		return static_cast<int2048>(0);
	int2048 a0=a,d;
	a0.symbol=true;
	d.symbol=a.symbol^b.symbol^1;
	d.Max_length=a.Max_length-b.Max_length+1;
	for(int i=d.Max_length-1;i>=0;i--)
	{
		int2048 b0;
		for(int j=0;j<b.Max_length;j++)
			b0.num[j+i]=b.num[j];
		b0.Max_length=b.Max_length+i;
		int l=0,r=1e9-1,mid;
		while(l<r)
		{
			mid=(l+r+1)/2;
			if(b0*mid<=a0)
				l=mid;
			else
				r=mid-1;
		}
		a0-=b0*l;
		d.num[i]=l;
	}
	d.simplify();
	return d;
}
inline int2048 operator% (const int2048 &a,const int &b)
{
	if(!b)
		throw std::runtime_error("The modulus can't be zero!");
	return a-(a/b)*b;
}
inline int2048 operator% (const int2048 &a,const int2048 &b)
{
	if(!b.Max_length)
		throw std::runtime_error("The modulus can't be zero!");
	return a-(a/b)*b;
}
inline int2048 operator^ (const int2048 &a,const int2048 &b)
{
	int2048 ans(1),x(a),y(b);
	while(y)
	{
		if(y.parity())
			ans*=x;
		x=x*x;
		y/=2;
	}
	return ans;
}
inline std::istream& operator>> (std::istream& in,int2048& a)
{
	a=int2048();
	std::string s;
	in>>s;
	a=int2048(s);
	return in;
}
inline std::ostream& operator<< (std::ostream& out,const int2048& a){return out<<std::string(a);}
inline bool operator== (const int2048 &a,const int2048 &b)
{
	if(!a.Max_length&&!b.Max_length)
		return true;
	if(a.symbol!=b.symbol)
		return false;
	if(a.Max_length!=b.Max_length)
		return false;
	for(int i=a.Max_length-1;i>=0;i--)
		if(a.num[i]!=b.num[i])
			return false;
	return true;
}
inline std::strong_ordering operator<=> (const int2048 &a,const int2048 &b)
{
	int sym_a=a.symbol?1:-1,sym_b=b.symbol?1:-1;
	if(!a.Max_length)
		sym_a=0;
	if(!b.Max_length)
		sym_b=0;
	if(!sym_a&&!sym_b)
		return std::strong_ordering::equal;
	if(sym_a!=sym_b)
	{
		if(sym_a>sym_b)
			return std::strong_ordering::greater;
		return std::strong_ordering::less;
	}
	const int Max=std::max(a.Max_length,b.Max_length);
	if(sym_a>0)
	{
		if(a.Max_length!=b.Max_length)
		{
			if(a.Max_length>b.Max_length)
				return std::strong_ordering::greater;
			return std::strong_ordering::less;
		}
		for(int i=Max-1;i>=0;i--)
			if(a.num[i]!=b.num[i])
			{
				if(a.num[i]>b.num[i])
					return std::strong_ordering::greater;
				return std::strong_ordering::less;
			}
		return std::strong_ordering::equal;
	}
	if(a.Max_length!=b.Max_length)
	{
		if(a.Max_length<b.Max_length)
			return std::strong_ordering::greater;
		return std::strong_ordering::less;
	}
	for(int i=Max-1;i>=0;i--)
		if(a.num[i]!=b.num[i])
		{
			if(a.num[i]>b.num[i])
				return std::strong_ordering::less;
			return std::strong_ordering::greater;
		}
	return std::strong_ordering::equal;
}
inline int2048::operator int()const
{
	if(Max_length>2)
		throw std::overflow_error("The value is beyond the range of int!");
	long long tmp=num[0]+1ll*MAX_NUM*num[1];
	if(!symbol)
		tmp=-tmp;
	if(tmp>INT_MAX||tmp<INT_MIN)
		throw std::overflow_error("The value is beyond the range of int!");
	return static_cast<int>(tmp);
}
inline int2048::operator long long()const
{
	if(Max_length>3)
		throw std::overflow_error("The value is beyond the range of long long!");
	__int128 tmp=num[0]+1ll*MAX_NUM*num[1]+static_cast<__int128>(MAX_NUM)*MAX_NUM*num[2];
	if(!symbol)
		tmp=-tmp;
	if(tmp>LONG_LONG_MAX||tmp<LONG_LONG_MIN)
		throw std::overflow_error("The value is beyond the range of long long!");
	return static_cast<long long>(tmp);
}
inline int2048::operator std::string()const
{
	if(!Max_length)
		return "0";
	std::string s;
	if(!symbol)
		s+='-';
	s+=std::to_string(num[Max_length-1]);
	for(int i=Max_length-2;i>=0;i--)
	{
		for(int j=MAX_DIGIT-1;j>=1;j--)
			if(num[i]<M_PW10[j])
				s+='0';
		s+=std::to_string(num[i]);
	}
	return s;
}

inline int2048 abs(const int2048 &a){return a>=int2048(0)?a:-a;}
inline int2048 gcd(int2048 a,int2048 b)
{
	if(!a||!b)
		return a+b;
	if(a<int2048(0))
		a=-a;
	if(b<int2048(0))
		b=-b;
	int2048 d(1);
	while(a.Max_length&&b.Max_length)
	{
		while(!(a.num[0]&1)&&!(b.num[0]&1))
			d*=2,a/=2,b/=2;
		while(!(a.num[0]&1))
			a/=2;
		while(!(b.num[0]&1))
			b/=2;
		if(a<=b)
			std::swap(a,b);
		a-=b;
	}
	return d*(a+b);
}
inline int2048 lcm(const int2048 &a,const int2048 &b){return a*b/gcd(a,b);}



/*
*It is a reinforced edition whose memory is dynamic.
*It supports nlogn multiplication and division.
*But it may perform not that well when dealing with small number(compared with int2048)
*/

constexpr int M_MAX_NUM=1e9;
constexpr int M_MAX_DIGIT=9;
const double pi=acos(-1.0);

inline void FFT_(std::complex<double> *p,const int &op,const int &n)
{
	std::vector r(n,0);
	const int l=static_cast<int>(log2(n));
	for(int i=0;i<n;i++)
		r[i]=(r[i>>1]>>1)|((i&1)<<(l-1));
	for(int i=0;i<n;i++)
		if(i<r[i])
			std::swap(p[i],p[r[i]]);
	for(int i=1;i<n;(i<<=1))
	{
		const std::complex w_n(cos(pi/i),op*sin(pi/i));
		for(int j=0;j<n;j+=(i<<1))
		{
			std::complex<double> w(1,0),t1,t2;
			for(int k=0;k<i;k++,w*=w_n)
				t1=p[j+k],t2=p[j+k+i]*w,p[j+k]=t1+t2,p[j+k+i]=t1-t2;
		}
	}
	if(op==-1)
		for(int i=0;i<n;i++)
			p[i]/=std::complex<double>(n,0);
}

class int65536
{
	friend class int2048;
	friend int65536 Right_Move_ (const int65536&,const int&);
	friend int65536 Left_Move_ (const int65536&,const int&);
	friend int65536 Move_ (const int65536&,const int&);
	friend int Count_Digit_ (const int65536&);
	friend int65536 operator+ (const int65536&);
	friend int65536 operator+ (const int65536&,const int65536&);
	friend int65536 operator- (const int65536&);
	friend int65536 operator- (const int65536&,const int65536&);
	friend int65536 operator* (const int65536&,const int&);
	friend int65536 operator* (const int&,const int65536&);
	friend int65536 operator* (const int65536&,const int65536&);
	friend int65536 operator/ (const int65536&,const int&);
	friend int65536 operator/ (const int65536&,const int65536&);
	friend int65536 operator% (const int65536&,const int&);
	friend int65536 operator% (const int65536&,const int65536&);
	friend int65536 operator^ (const int65536&,const int65536&);
	friend std::istream& operator>> (std::istream&,int65536&);
	friend std::ostream& operator<< (std::ostream&,const int65536&);
	friend bool operator== (const int65536&,const int65536&);
	friend std::strong_ordering operator<=> (const int65536&,const int65536&);
	friend int65536 gcd(int65536 a,int65536 b);
private:
	std::vector<int> num;
	bool symbol=true;
	void simplify(){while(!num.empty()&&!num.back())num.pop_back();}
public:
	int65536(){num.clear();}
	explicit int65536(const int &a);
	explicit int65536(const long long &a);
	explicit int65536(const std::string &s);
	int65536(const int65536 &a);
	int65536(int65536 &&a)noexcept:num(std::move(a.num)),symbol(a.symbol){}
	explicit int65536(const int2048 &a);
	[[nodiscard]] int sgn() const{return symbol;}
	[[nodiscard]] int parity() const{return num.empty()?0:num[0]&1;}
	[[nodiscard]] int at(const int &x) const;
	int65536& operator=(const int65536 &a);
	int65536& operator+=(const int65536&);
	int65536& operator-=(const int65536&);
	int65536& operator*=(const int&);
	int65536& operator*=(const int65536&);
	int65536& operator/=(const int&);
	int65536& operator/=(const int65536&);
	int65536& operator%=(const int&);
	int65536& operator%=(const int65536&);
	int65536& operator^=(const int65536&);
	int65536& operator++ ();
	int65536 operator++ (int tmp);
	int65536& operator-- ();
	int65536 operator-- (int tmp);
	explicit operator bool()const{return !num.empty();}
	explicit operator int()const;
	explicit operator long long()const;
	explicit operator std::string()const;
};

inline int65536::int65536(const int &a)
{
	unsigned int tmp;
	if(a<0)
		symbol=false,tmp=-a;
	else
		tmp=a;
	num.push_back(static_cast<int>(tmp%M_MAX_NUM));
	num.push_back(static_cast<int>(tmp/M_MAX_NUM));
	simplify();
}
inline int65536::int65536(const long long &a)
{
	unsigned long long tmp=a;
	if(a<0)
		symbol=false,tmp=-tmp;
	num.push_back(static_cast<int>(tmp%M_MAX_NUM));
	if(tmp>=M_MAX_NUM)
	{
		tmp/=M_MAX_NUM;
		num.push_back(static_cast<int>(tmp%M_MAX_NUM));
		if(tmp>=M_MAX_NUM)
		{
			tmp/=M_MAX_NUM;
			num.push_back(static_cast<int>(tmp%M_MAX_NUM));
			if(tmp>=M_MAX_NUM)
				num.push_back(static_cast<int>(tmp/M_MAX_NUM));
		}
	}
	simplify();
}
inline int65536::int65536(const std::string &s)
{
	for(int i=0;i<static_cast<int>(s.size());i++)
	{
		if(s[i]!='-'&&s[i]!='+'&&(s[i]<'0'||s[i]>'9'))
		{
#ifdef INTELLIGENT_H
			throw invalid_expression("Illegal symbol!");
#endif
			return ;
		}
		if(s[i]=='-'&&i)
		{
#ifdef INTELLIGENT_H
			throw invalid_expression("Illegal symbol!");
#endif
			return ;
		}
		if(s[i]=='+'&&i)
		{
#ifdef INTELLIGENT_H
			throw invalid_expression("Illegal symbol!");
#endif
			return ;
		}
	}
	int l=static_cast<int>(s.size()),r,fi=0;
	if(!s.empty()&&s[0]=='-')
		symbol=false,fi=1;
	if(!s.empty()&&s[0]=='+')
		fi=1;
	num.reserve(s.size()/M_MAX_DIGIT+1);
	while(l!=fi)
	{
		int tmp=0;
		r=l,l=r-M_MAX_DIGIT;
		if(l<fi)
			l=fi;
		for(int i=l;i<r;i++)
			tmp=tmp*10+s[i]-'0';
		num.emplace_back(tmp);
	}
	simplify();
}
inline int65536::int65536(const int65536 &a)
{
	symbol=a.symbol;
	num=a.num;
}
inline int int65536::at(const int &x) const
{
	if(x<=0||x>num.size()*M_MAX_DIGIT)
		return 0;
	const int pos=(x-1)/M_MAX_DIGIT,res=x-pos*M_MAX_DIGIT;
	return num[pos]/M_PW10[res-1]%10;
}
inline int65536& int65536::operator=(const int65536 &a)
{
	if(this==&a)
		return *this;
	num=a.num;symbol=a.symbol;
	return *this;
}
inline int65536& int65536::operator+=(const int65536 &a){return *this=*this+a;}
inline int65536& int65536::operator-=(const int65536 &a){return *this=*this-a;}
inline int65536& int65536::operator*=(const int &a){return *this=*this*a;}
inline int65536& int65536::operator*=(const int65536 &a){return *this=*this*a;}
inline int65536& int65536::operator/=(const int &a){return *this=*this/a;}
inline int65536& int65536::operator/=(const int65536 &a){return *this=*this/a;}
inline int65536& int65536::operator%=(const int &a){return *this=*this%a;}
inline int65536& int65536::operator%=(const int65536 &a){return *this=*this%a;}
inline int65536& int65536::operator^=(const int65536 &a){return *this=*this^a;}
inline int65536& int65536::operator++(){return *this=*this+int65536(1);}
inline int65536 int65536::operator++(int tmp){int65536 a=*this;*this=*this+int65536(1);return a;}
inline int65536& int65536::operator--(){return *this=*this-int65536(1);}
inline int65536 int65536::operator--(int tmp){int65536 a=*this;*this=*this-int65536(1);return a;}

inline int65536 Right_Move_(const int65536 &a,const int &t)
{
	if(t>=a.num.size()*M_MAX_DIGIT)
		return int65536(0);
	if(!t)
		return a;
	int65536 b;
	b.symbol=a.symbol;
	const int st=t/M_MAX_DIGIT,del=t-st*M_MAX_DIGIT,pw=M_PW10[del],ip=M_MAX_NUM/pw;
	b.num.resize(a.num.size()-st+1,0);
	for(int i=0;i+st<a.num.size()-1;i++)
		b.num[i]=a.num[st+i]/pw+a.num[st+i+1]%pw*ip;
	b.num[a.num.size()-1-st]=a.num.back()/pw;
	b.simplify();
	return b;
}
inline int65536 Left_Move_(const int65536 &a,const int &t)
{
	if(a.num.empty())
		return int65536(0);
	if(!t)
		return a;
	int65536 b;
	b.symbol=a.symbol;
	const int st=t/M_MAX_DIGIT,del=t-st*M_MAX_DIGIT,pw=M_PW10[del],ip=M_MAX_NUM/pw;
	b.num.resize(a.num.size()+st+1,0);
	b.num[st]=a.num[0]%ip*pw;
	for(int i=1;i<a.num.size();i++)
		b.num[st+i]=a.num[i-1]/ip+a.num[i]%ip*pw;
	b.num[st+a.num.size()]=a.num.back()/ip;
	b.simplify();
	return b;
}
inline int65536 Move_(const int65536 &a,const int &t)
{
	if(!t)
		return a;
	if(t>0)
		return Left_Move_(a,t);
	return Right_Move_(a,-t);
}
inline int Count_Digit_(const int65536 &a)
{
	if(a.num.empty())
		return 0;
	int ans=static_cast<int>(a.num.size()-1)*M_MAX_DIGIT,val=a.num.back();
	while(val)
		val/=10,ans++;
	return ans;
}

inline int65536 operator+ (const int65536 &a){return a;}
inline int65536 operator+ (const int65536 &a,const int65536 &b)
{
	int65536 c;c.num.resize(std::max(a.num.size(),b.num.size())+1,0);c.symbol=a.symbol;
	if(a.symbol==b.symbol)
	{
		memcpy(c.num.data(),a.num.data(),sizeof(int)*a.num.size());
		for(int i=0;i<b.num.size();i++)
			c.num[i]+=b.num[i];
		for(int i=0;i<c.num.size()-1;i++)
			if(c.num[i]>=M_MAX_NUM)
				c.num[i]-=M_MAX_NUM,c.num[i+1]++;
		c.simplify();
	}
	else
	{
		memcpy(c.num.data(),a.num.data(),sizeof(int)*a.num.size());
		for(int i=0;i<b.num.size();i++)
			c.num[i]-=b.num[i];
		for(int i=0;i<c.num.size()-1;i++)
			if(c.num[i]<0)
				c.num[i]+=M_MAX_NUM,c.num[i+1]--;
		c.simplify();
		if(!c.num.empty()&&c.num.back()<0)
		{
			c.symbol^=1;
			for(int i=static_cast<int>(c.num.size()-1);i>=0;i--)
				c.num[i]=-c.num[i];
			for(int i=0;i<static_cast<int>(c.num.size());i++)
				if(c.num[i]<0)
					c.num[i+1]--,c.num[i]+=M_MAX_NUM;
			c.simplify();
		}
	}
	return c;
}
inline int65536 operator- (const int65536 &a)
{
	int65536 b(a);
	b.symbol^=1;
	return b;
}
inline int65536 operator- (const int65536 &a,const int65536 &b){return a+(-b);}
inline int65536 operator* (const int65536 &a,const int &b)
{
	long long carry=0;
	int65536 c;
	c.symbol=a.symbol;
	c.num.resize(a.num.size(),0);
	long long t=b;
	if(b<0)
		t=-t,c.symbol^=1;
	for(int i=0;i<static_cast<int>(a.num.size());i++)
	{
		const long long tmp=1ll*a.num[i]*t+carry;
		carry=static_cast<int>(tmp/M_MAX_NUM);
		c.num[i]=static_cast<int>(tmp-1ll*carry*M_MAX_NUM);
	}
	while(carry)
	{
		const long long tmp=carry/M_MAX_NUM;
		c.num.emplace_back(static_cast<int>(carry-tmp*M_MAX_NUM));
		carry=tmp;
	}
	c.simplify();
	return c;
}
inline int65536 operator* (const int &a,const int65536 &b){return b*a;}
inline int65536 operator* (const int65536 &a,const int65536 &b)
{
	if(a.num.size()<=32||b.num.size()<=32)
	{
		int65536 c;
		c.symbol=a.symbol^b.symbol^1;
		c.num.resize(a.num.size()+b.num.size(),0);
		for(int i=0;i<a.num.size();i++)
		{
			const int65536 tmp=b*a.num[i];
			for(int j=0;j<tmp.num.size();j++)
				c.num[i+j]=c.num[i+j]+tmp.num[j];
			for(int j=i;j<tmp.num.size()+i;j++)
				if(c.num[j]>=M_MAX_NUM)
					c.num[j]-=M_MAX_NUM,c.num[j+1]++;
		}
		for(int i=0;i<c.num.size();i++)
			if(c.num[i]>=M_MAX_NUM)
				c.num[i]-=M_MAX_NUM,c.num[i+1]++;
		c.simplify();
		return c;
	}
	const int siz=static_cast<int>(a.num.size()+b.num.size())*3;
	int n=1;
	while(n<=siz)
		n*=2;
	auto *fa=new std::complex<double>[n];
	constexpr int D=1e3;
	for(int i=0;i<static_cast<int>(a.num.size());i++)
	{
		const int x=a.num[i]/(D*D),y=a.num[i]/D%D,z=a.num[i]%D;
		fa[i*3]+=std::complex<double>(z,0);
		fa[i*3+1]+=std::complex<double>(y,0);
		fa[i*3+2]+=std::complex<double>(x,0);
	}
	for(int i=0;i<static_cast<int>(b.num.size());i++)
	{
		const int x=b.num[i]/(D*D),y=b.num[i]/D%D,z=b.num[i]%D;
		fa[i*3]+=std::complex<double>(0,z);
		fa[i*3+1]+=std::complex<double>(0,y);
		fa[i*3+2]+=std::complex<double>(0,x);
	}
	FFT_(fa,1,n);
	for(int i=0;i<n;i++)
		fa[i]*=fa[i];
	FFT_(fa,-1,n);
	long long carry=0;
	int65536 c;c.num.resize(n,0);
	c.symbol=a.symbol^b.symbol^1;
	for(int i=0;i<n;i++)
	{
		const long long tmp=carry+static_cast<long long>(roundl(fa[i].imag()/2.0));
		carry=tmp/D;
		int cnt=1;
		if(i%3==1)
			cnt=D;
		else if(i%3==2)
			cnt=D*D;
		c.num[i/3]+=cnt*static_cast<int>(tmp-carry*D);
	}
	c.simplify();
	delete[] fa;
	return c;
}
inline int65536 operator/ (const int65536 &a,const int &b)
{
	if(!b)
		throw std::runtime_error("The divisor can't be zero!");
	int65536 c;
	c.symbol=a.symbol;
	c.num.resize(a.num.size(),0);
	long long tmp=b;
	if(b<0)
		tmp=-tmp,c.symbol^=1;
	int carry=0;
	for(int i=static_cast<int>(c.num.size()-1);i>=0;i--)
	{
		const long long now=1ll*carry*M_MAX_NUM+a.num[i];
		c.num[i]=static_cast<int>(now/tmp);
		carry=static_cast<int>(now-1ll*c.num[i]*tmp);
	}
	c.simplify();
	return c;
}
inline int65536 operator/ (const int65536 &a,const int65536 &b)
{
	if(b.num.empty())
		throw std::runtime_error("The divisor can't be zero!");
	int dig_a=Count_Digit_(a),dig_b=Count_Digit_(b);
	if(dig_a<dig_b)
	{
		if(!dig_a||a.sgn()==b.sgn())
			return int65536(0);
		return int65536(-1);
	}
	if(a.num.size()-b.num.size()<=32)
	{
		int65536 a0=a,d;
		a0.symbol=true;
		d.symbol=a.symbol^b.symbol^1;
		d.num.resize(a.num.size()-b.num.size()+1,0);
		for(int i=static_cast<int>(d.num.size()-1);i>=0;i--)
		{
			int65536 b0;b0.num.resize(b.num.size()+i,0);
			for(int j=0;j<b.num.size();j++)
				b0.num[j+i]=b.num[j];
			int l=0,r=M_MAX_NUM-1,mid;
			while(l<r)
			{
				mid=(l+r+1)/2;
				if(b0*mid<=a0)
					l=mid;
				else
					r=mid-1;
			}
			a0-=b0*l;
			d.num[i]=l;
		}
		if(!b.symbol&&d*b<a)
			d--;
		if(b.symbol&&d*b>a)
			d--;
		d.simplify();
		return d;
	}
	const int sym=a.symbol^b.symbol^1;
	int65536 ta=a,tb=b;
	const auto one=int65536(1);
	ta.symbol=tb.symbol=true;
	if(dig_a>2*dig_b)
	{
		const int t=dig_a-2*dig_b;
		ta=Left_Move_(ta,t),tb=Left_Move_(tb,t);
		dig_b+=t;
	}
	int n=1;
	while(n<2*dig_b)
		n*=2;
	int N[26],cnt=0,nn=dig_b;
	while(nn>2)
		N[cnt++]=nn,nn=nn/2+1;
	N[cnt++]=nn;
	auto q=int65536(M_PW10[N[cnt-1]<<1]/Right_Move_(tb,dig_b-N[cnt-1]).num[0]);
	for(int i=cnt-2;i>=0;i--)
		q=Left_Move_(2*q,N[i]-N[i+1])-one-Right_Move_(q*q*Right_Move_(tb,dig_b-N[i])-one,N[i+1]<<1);
	const int65536 c=Left_Move_(one,dig_b<<1)-q*tb;
	int l=0,r=100;
	while(l<r)
	{
		const int mid=(l+r+1)>>1;
		if(tb*mid<=c)
			l=mid;
		else
			r=mid-1;
	}
	q+=int65536(l);
	int65536 res=Right_Move_(q*ta,dig_b<<1);
	ta-=tb*res;
	while(tb<=ta)
		++res,ta-=tb;
	res.symbol=sym;
	if(!b.symbol&&res*b<a)
		res--;
	if(b.symbol&&res*b>a)
		res--;
	return res;
}
inline int65536 operator% (const int65536 &a,const int &b)
{
	if(!b)
		throw std::runtime_error("The modulus can't be zero!");
	const auto c=a/b*b;
	return a-c;
}
inline int65536 operator% (const int65536 &a,const int65536 &b)
{
	if(b.num.empty())
		throw std::runtime_error("The modulus can't be zero!");
	return a-(a/b)*b;
}
inline int65536 operator^ (const int65536 &a,const int65536 &b)
{
	int65536 ans(1),x(a),y(b);
	while(y)
	{
		if(y.parity())
			ans*=x;
		x=x*x;
		y/=2;
	}
	return ans;
}
inline std::istream& operator>> (std::istream& in,int65536& a)
{
	std::string s;
	in>>s;
	a=int65536(s);
	return in;
}
inline std::ostream& operator<< (std::ostream& out,const int65536& a){return out<<std::string(a);}
inline bool operator== (const int65536 &a,const int65536 &b)
{
	if(a.num.empty()&&b.num.empty())
		return true;
	if(a.symbol!=b.symbol)
		return false;
	if(a.num.size()!=b.num.size())
		return false;
	for(int i=static_cast<int>(a.num.size())-1;i>=0;i--)
		if(a.num[i]!=b.num[i])
			return false;
	return true;
}
inline std::strong_ordering operator<=> (const int65536 &a,const int65536 &b)
{
	int sym_a=a.symbol?1:-1,sym_b=b.symbol?1:-1;
	if(a.num.empty())
		sym_a=0;
	if(b.num.empty())
		sym_b=0;
	if(!sym_a&&!sym_b)
		return std::strong_ordering::equal;
	if(sym_a!=sym_b)
	{
		if(sym_a>sym_b)
			return std::strong_ordering::greater;
		return std::strong_ordering::less;
	}
	const int Max=static_cast<int>(std::max(a.num.size(),b.num.size()));
	if(sym_a>0)
	{
		if(a.num.size()!=b.num.size())
		{
			if(a.num.size()>b.num.size())
				return std::strong_ordering::greater;
			return std::strong_ordering::less;
		}
		for(int i=Max-1;i>=0;i--)
			if(a.num[i]!=b.num[i])
			{
				if(a.num[i]>b.num[i])
					return std::strong_ordering::greater;
				return std::strong_ordering::less;
			}
		return std::strong_ordering::equal;
	}
	if(a.num.size()!=b.num.size())
	{
		if(a.num.size()<b.num.size())
			return std::strong_ordering::greater;
		return std::strong_ordering::less;
	}
	for(int i=Max-1;i>=0;i--)
		if(a.num[i]!=b.num[i])
		{
			if(a.num[i]>b.num[i])
				return std::strong_ordering::less;
			return std::strong_ordering::greater;
		}
	return std::strong_ordering::equal;
}
inline int65536::operator int()const
{
	if(num.size()>2)
		throw std::overflow_error("The value is beyond the range of int!");
	if(num.empty())
		return 0;
	if(num.size()==1)
		return symbol?num[0]:-num[0];
	long long tmp=num[0]+1ll*M_MAX_NUM*num[1];
	if(!symbol)
		tmp=-tmp;
	if(tmp>INT_MAX||tmp<INT_MIN)
		throw std::overflow_error("The value is beyond the range of int!");
	return static_cast<int>(tmp);
}
inline int65536::operator long long()const
{
	if(num.size()>3)
		throw std::overflow_error("The value is beyond the range of long long!");
	if(num.size()<=2)
	{
		long long ans=0;
		for(int i=static_cast<int>(num.size())-1;i>=0;i--)
			ans=ans*M_MAX_NUM+num[i];
		if(!symbol)
			ans=-ans;
		return ans;
	}
	__int128 tmp=num[0]+1ll*M_MAX_NUM*num[1]+static_cast<__int128>(M_MAX_NUM)*M_MAX_NUM*num[2];
	if(!symbol)
		tmp=-tmp;
	if(tmp>LONG_LONG_MAX||tmp<LONG_LONG_MIN)
		throw std::overflow_error("The value is beyond the range of long long!");
	return static_cast<long long>(tmp);
}
inline int65536::operator std::string()const
{
	if(num.empty())
		return "0";
	std::string s;
	if(!symbol)
		s+='-';
	s+=std::to_string(num.back());
	for(int i=static_cast<int>(num.size())-2;i>=0;i--)
	{
		for(int j=M_MAX_DIGIT-1;j>=1;j--)
			if(num[i]<M_PW10[j])
				s+='0';
		s+=std::to_string(num[i]);
	}
	return s;
}

inline int65536 abs(const int65536 &a){return a>=int65536(0)?a:-a;}
inline int65536 gcd(int65536 a,int65536 b)
{
	if(!a||!b)
		return a+b;
	if(a<int65536(0))
		a=-a;
	if(b<int65536(0))
		b=-b;
	int65536 d(1);
	while(!a.num.empty()&&!b.num.empty())
	{
		while(!(a.num[0]&1)&&!(b.num[0]&1))
			d*=2,a/=2,b/=2;
		while(!(a.num[0]&1))
			a/=2;
		while(!(b.num[0]&1))
			b/=2;
		if(a<=b)
			std::swap(a,b);
		a-=b;
	}
	return d*(a+b);
}
inline int65536 lcm(const int65536 &a,const int65536 &b){return a*b/gcd(a,b);}

inline int2048::int2048(const int65536 &a)
{
	const int digit=static_cast<int>(a.num.size())*M_MAX_DIGIT;
	if(digit>2160)
		throw std::overflow_error("The value is beyond the range of int2048!");
	symbol=a.symbol;
	const auto d=new int[digit];int cnt=0;
	for(int x:a.num)
		for(int j=1;j<=M_MAX_DIGIT;j++)
			d[cnt++]=x%10,x/=10;
	cnt=0;
	while(cnt<digit)
	{
		Max_length++;
		for(int i=std::min(MAX_DIGIT,digit-cnt)-1;i>=0;i--)
			num[Max_length-1]=num[Max_length-1]*10+d[cnt+i];
		cnt+=MAX_DIGIT;
	}
	simplify();
	delete[] d;
}
inline int65536::int65536(const int2048 &a)
{
	const int digit=a.Max_length*MAX_DIGIT;
	symbol=a.symbol;
	const auto d=new int[digit];int cnt=0;
	for(int i=0;i<a.Max_length;i++)
	{
		int x=a.num[i];
		for(int j=1;j<=MAX_DIGIT;j++)
			d[cnt++]=x%10,x/=10;
	}
	cnt=0;
	while(cnt<digit)
	{
		num.emplace_back(0);
		for(int i=std::min(M_MAX_DIGIT,digit-cnt)-1;i>=0;i--)
			num[num.size()-1]=num[num.size()-1]*10+d[cnt+i];
		cnt+=M_MAX_DIGIT;
	}
	simplify();
	delete[] d;
}

#endif
