#ifndef FLOAT2048_H
#define FLOAT2048_H

#include"int2048.h"

template<int siz=20> class float2048;

inline int LAS_=-1,EXPO_;
inline int65536 VAL_;

template<int siz> float2048<siz> Move_(const float2048<siz> &a,const int &b)
{
	if(!a.val)
		return a;
	float2048 ans=a;ans.expo+=b;
	return ans;
}

template<int siz> float2048<siz> operator+ (const float2048<siz> &a){return a;}
template<int siz> float2048<siz> operator+ (const float2048<siz> &a,const float2048<siz> &b)
{
	int new_expo=std::max(a.expo,b.expo);
	float2048<siz> c;c.expo=new_expo;
	if(a.expo!=new_expo)
	{
		if(new_expo-a.expo>=siz*M_MAX_DIGIT)
			c.val=b.val;
		else
			c.val=b.val+Right_Move_(a.val,new_expo-a.expo);
	}
	else
	{
		if(new_expo-b.expo>=siz*M_MAX_DIGIT)
			c.val=a.val;
		else
			c.val=a.val+Right_Move_(b.val,new_expo-b.expo);
	}
	c.simplify();
	return c;
}
template<int siz> float2048<siz> operator- (const float2048<siz> &a){float2048<siz> tmp=a;tmp.val=-tmp.val;return tmp;}
template<int siz> float2048<siz> operator- (const float2048<siz> &a,const float2048<siz> &b){return a+(-b);}
template<int siz> float2048<siz> operator* (const float2048<siz> &a,const float2048<siz> &b)
{
	float2048<siz> c;c.expo=a.expo+b.expo;
	c.val=a.val*b.val;
	c.simplify();
	return c;
}
template<int siz> float2048<siz> operator/ (const float2048<siz> &a,const float2048<siz> &b)
{
	if(!b.val)
		throw std::runtime_error("The divisor can't be zero!");
	float2048<siz> c;c.expo=a.expo-b.expo-siz*M_MAX_DIGIT;
	c.val=Left_Move_(a.val,siz*M_MAX_DIGIT)/b.val;
	c.simplify();
	return c;
}
template<int siz> float2048<siz> operator% (const float2048<siz> &a,const float2048<siz> &b)
{
	auto tmp=fmod(a,b);
	if(tmp<float2048<siz>(0))
		tmp+=b;
	return tmp;
}
template<int siz> float2048<siz> operator^ (const float2048<siz> &a,const float2048<siz> &b)
{
	return pow(a,b);
}
template<int siz> std::istream& operator>>(std::istream &in,float2048<siz> &a)
{
	std::string s;
	in>>s;
	a=float2048<siz>(s);
	return in;
}
template<int siz> std::ostream& operator<<(std::ostream &out,const float2048<siz> &a)
{
	if(!a.val.sgn())
		out<<"-";
	int t=static_cast<int>(out.precision());
	if(out.flags()&std::ostream::scientific)
	{
		t=std::min(t,siz*M_MAX_DIGIT),t=std::max(t,1);
		int65536 fixed_val=abs(Right_Move_(a.val,siz*M_MAX_DIGIT-t));
		if(a.val.at(siz*M_MAX_DIGIT-t)==5)
		{
			const int65536 rest_val=abs(a.val)-Left_Move_(fixed_val,siz*M_MAX_DIGIT-t);
			if(rest_val!=Left_Move_(static_cast<int65536>(5),siz*M_MAX_DIGIT-t-1)||fixed_val.parity())
				++fixed_val;
		}
		else if(a.val.at(siz*MAX_DIGIT-t)>5)
			++fixed_val;
		int expo0=siz*M_MAX_DIGIT-1+a.expo;
		if(fixed_val==Left_Move_(int65536(1),t))
			++expo0,fixed_val=Right_Move_(fixed_val,1);
		out<<fixed_val.at(t);
		if(t!=1)
			out<<'.';
		for(int i=t-1;i>=1;--i)
			out<<fixed_val.at(i);
		putchar('e');
		if(expo0>=0)
			out<<'+'<<expo0;
		else
			out<<expo0;
		return out;
	}
	if(out.flags()&std::ostream::fixed)
	{
		t=std::max(t,0);
		int65536 left_val,right_val;
		if(a.expo>=0)
			left_val=abs(Left_Move_(a.val,a.expo));
		else
		{
			left_val=abs(Right_Move_(a.val,-a.expo));
			right_val=abs(a.val)-Left_Move_(left_val,-a.expo);
			if(-a.expo>t)
			{
				int65536 pre_val=right_val;
				right_val=Right_Move_(right_val,-a.expo-t);
				if(pre_val.at(-a.expo-t)>5)
					++right_val;
				else if(pre_val.at(-a.expo-t)==5)
				{
					int65536 rest_val=pre_val-Left_Move_(right_val,-a.expo-t);
					if(rest_val!=Left_Move_(int65536(5),-a.expo-t-1)||right_val.parity())
						++right_val;
				}
				if(right_val==Left_Move_(int65536(1),t))
					right_val=int65536(0),++left_val;
			}
			else if(-a.expo<=t)
				right_val=Left_Move_(right_val,t+a.expo);
		}
		out<<left_val;
		if(t)
			out<<'.';
		for(int i=t;i>=1;--i)
			out<<right_val.at(i);
		return out;
	}
	t=std::min(t,siz*M_MAX_DIGIT),t=std::max(t,1);
	int65536 fixed_val=abs(Right_Move_(a.val,siz*M_MAX_DIGIT-t));
	if(a.val.at(siz*M_MAX_DIGIT-t)==5)
	{
		const int65536 rest_val=abs(a.val)-Left_Move_(fixed_val,siz*M_MAX_DIGIT-t);
		if(rest_val!=Left_Move_(static_cast<int65536>(5),siz*M_MAX_DIGIT-t-1)||fixed_val.parity())
			++fixed_val;
	}
	else if(a.val.at(siz*MAX_DIGIT-t)>5)
		++fixed_val;
	int expo0=siz*M_MAX_DIGIT-1+a.expo;
	if(fixed_val==Left_Move_(int65536(1),t))
		++expo0,fixed_val=Right_Move_(fixed_val,1);
	for(int i=1;i<=t;i++)
		if(fixed_val.at(i)||i==t)
		{
			fixed_val=Right_Move_(fixed_val,i-1);
			t-=i-1;
			break;
		}
	if(expo0>=static_cast<int>(out.precision())||expo0<=-2)
	{
		out<<fixed_val.at(t);
		if(t-1>=1)
			out<<'.';
		for(int i=t-1;i>=1;--i)
			out<<fixed_val.at(i);
		putchar('e');
		if(expo0>=0)
			out<<'+'<<expo0;
		else
			out<<expo0;
	}
	else if(expo0==-1)
	{
		out<<"0.";
		if(t>=1)
			for(int i=t;i>=1;--i)
				out<<fixed_val.at(i);
		else
			out<<"0";
	}
	else
	{
		for(int i=t;i>=t-expo0;--i)
			out<<fixed_val.at(i);
		out<<'.';
		if(t-expo0>=2)
			for(int i=t-expo0-1;i>=1;--i)
				out<<fixed_val.at(i);
		else
			out<<'0';
	}
	return out;
}
template<int siz> bool operator==(const float2048<siz> &a,const float2048<siz> &b)
{
	int sa=a.val.sgn()?1:-1,sb=b.val.sgn()?1:-1;
	if(!a.val)
		sa=0;
	if(!b.val)
		sb=0;
	if(sa!=sb)
		return false;
	if(!sa&&!sb)
		return true;
	if(a.expo!=b.expo)
		return false;
	return a.val==b.val;
}
template<int siz> std::strong_ordering operator<=>(const float2048<siz> &a,const float2048<siz> &b)
{
	int sa=a.val.sgn()?1:-1,sb=b.val.sgn()?1:-1;
	if(!a.val)
		sa=0;
	if(!b.val)
		sb=0;
	if(sa<sb)
		return std::strong_ordering::less;
	if(sa>sb)
		return std::strong_ordering::greater;
	if(!sa)
		return std::strong_ordering::equal;
	if(sa==1)
	{
		if(a.expo<b.expo)
			return std::strong_ordering::less;
		if(a.expo>b.expo)
			return std::strong_ordering::greater;
		return a.val<=>b.val;
	}
	if(a.expo<b.expo)
		return std::strong_ordering::greater;
	if(a.expo>b.expo)
		return std::strong_ordering::less;
	return a.val<=>b.val;
}

template<int siz> float2048<siz> abs(const float2048<siz> &a){return a.val.sgn()?a:-a;}
template<int siz> float2048<siz> floor(const float2048<siz> &a)
{
	if(a.expo>=0)
		return a;
	float2048<siz> b;b.val=Right_Move_(a.val,-a.expo);b.expo=0;
	if(!a.val.sgn()&&Left_Move_(b.val,-a.expo)!=a.val)
		--b.val;
	b.simplify();
	return b;
}
template<int siz> float2048<siz> ceil(const float2048<siz> &a){return -floor(-a);}
template<int siz> float2048<siz> round(const float2048<siz> &a)
{
	if(a.expo>=0)
		return a;
	float2048<siz> b;b.val=abs(Right_Move_(a.val,-a.expo));b.expo=0;
	if(a.val.at(-a.expo)>=5)
		++b.val;
	b.simplify();
	return a.val.sgn()?b:-b;
}
template<int siz> float2048<siz> cround(const float2048<siz> &a)
{
	if(a.expo>=0)
		return a;
	float2048<siz> b;b.val=abs(Right_Move_(a.val,-a.expo));b.expo=0;
	if(a.val.at(-a.expo)>=6)
		++b.val;
	if(a.val.at(-a.expo)==5)
	{
		int65536 res=abs(a.val)-Left_Move_(b.val,-a.expo);
		if(b.val.parity()||res!=Left_Move_(int65536(5),-a.expo-1))
			++b.val;
	}
	b.simplify();
	return a.val.sgn()?b:-b;
}
template<int siz> float2048<siz> cut(const float2048<siz> &a)
{
	if(a.expo>=0)
		return a;
	float2048<siz> b;b.val=Right_Move_(a.val,-a.expo);b.expo=0;;
	b.simplify();
	return b;
}
template<int siz> bool is_integer(const float2048<siz> &a)
{
	if(a.expo>=0)
		return true;
	else if(a.expo<=-siz*M_MAX_DIGIT)
		return false;
	return Left_Move_(Right_Move_(a.val,-a.expo),-a.expo)==a.val;
}

template<int siz> float2048<siz> fmod(const float2048<siz> &a,const float2048<siz> &b)
{
	if(!b.val)
		throw std::runtime_error("The divisor can't be zero!");
	return a-cut(a/b)*b;
}
template<int siz> float2048<siz> remainder(const float2048<siz> &a,const float2048<siz> &b)
{
	if(!b.val)
		throw std::runtime_error("The divisor can't be zero!");
	return a-cround(a/b)*b;
}
template<int siz> float2048<siz> exp_(const float2048<siz> &a)
{
	if(a.val&&a.expo+siz*M_MAX_DIGIT-1>=9)
		throw std::domain_error("The exponent is too big!");
	if(!a.val)
		return float2048<siz>(1);
	float2048<siz+2> ans(0),now(1),x(a);int cnt=1;
	while(true)
	{
		ans+=now;
		now=now*x;
		now=now/float2048<siz+2>(cnt);
		cnt++;
		if(now.expo-ans.expo<-(siz+2)*M_MAX_DIGIT)
			return float2048<siz>(ans);
	}
}
template<int siz> float2048<siz> exp(const float2048<siz> &a)
{
	if(a.val&&a.expo+siz*M_MAX_DIGIT-1>=9)
		throw std::domain_error("The exponent is too big!");
	if(!a.val)
		return float2048<siz>(1);
	float2048<siz> base("0.1");
	if(!a.val.sgn())
		base=-base;
	auto t=static_cast<long long>(floor(a/base));float2048 remains=a-float2048<siz>(t)*base;
	if(!t)
		return exp_(remains);
	float2048<siz+2> ans=exp_(float2048<siz+2>(remains)),ans_base=exp_(float2048<siz+2>(base));
	while(t)
	{
		if(t&1)
			ans=ans*ans_base;
		t>>=1;
		ans_base*=ans_base;
	}
	return float2048<siz>(ans);
}
template<int siz> float2048<siz> ln_(const float2048<siz> &a)
{
	if(!a.val||!a.val.sgn())
		throw std::domain_error("Beyond the domain of definition!");
	float2048<siz+2> x=float2048<siz+2>(1)-float2048<siz+2>(a);
	if(!x.val)
		return float2048<siz>(0);
	float2048<siz+2> ans(0),now(1);int cnt=1;
	while(true)
	{
		now*=x;
		ans+=now;
		now*=float2048<siz+2>(cnt);
		cnt++;
		now/=float2048<siz+2>(cnt);
		if(now.expo-ans.expo<-(siz+2)*M_MAX_DIGIT)
			return -float2048<siz>(ans);
	}
}
template<int siz> float2048<siz> ln(const float2048<siz> &a)
{
	if(!a.val||!a.val.sgn())
		throw std::domain_error("Beyond the domain of definition!");
	long long t=-a.expo-siz*M_MAX_DIGIT+1;
	float2048<siz+2> v08=float2048<siz+2>("0.8"),ln08=ln_(v08);
	float2048<siz+2> x(a);int cnt=0,pre=x.expo;
	while(true)
	{
		float2048 p=x;
		x*=v08;cnt++;
		if(x.expo!=pre)
		{
			x=p;cnt--;
			break;
		}
	}
	if(!t)
		return float2048<siz>(-float2048<siz+2>(cnt)*ln08+ln_(x));
	float2048<siz+2> lnt=ln_(float2048<siz+2>("0.931322574615478515625"));
	x.expo=1-(siz+2)*M_MAX_DIGIT;
	return float2048<siz>(float2048<siz+2>(t)*lnt+float2048<siz+2>(10*t-cnt)*ln08+ln_(x));
}
template<int siz> float2048<siz> pow(const float2048<siz> &a,const float2048<siz> &b)
{
	if(!a.val)
	{
		if(!b.val)
			return float2048<siz>(1);
		if(b.val.sgn())
			return float2048<siz>(0);
		throw std::domain_error("Beyond the domain of definition!");
	}
	if(!a.val.sgn())
	{
		if(!is_integer(b))
			throw std::domain_error("Beyond the domain of definition!");
		return b.val.at(-b.expo)&1?-float2048<siz>(exp(float2048<siz+2>(b)*ln(float2048<siz+2>(-a)))):float2048<siz>(exp(float2048<siz+2>(b)*ln(float2048<siz+2>(-a))));
	}
	if(!a.val||!a.val.sgn())
		throw std::domain_error("Beyond the domain of definition!");
	return float2048<siz>(exp(float2048<siz+2>(b)*ln(float2048<siz+2>(a))));
}
template<int siz> float2048<siz> sqrt(const float2048<siz> &a)
{
	if(!a.val)
		return float2048<siz>(0);
	if(!a.val.sgn())
		throw std::domain_error("Beyond the domain of definition!");
	float2048<siz+2> x(a);
	int cnt=0,del=(1-(siz+2)*M_MAX_DIGIT-x.expo)/2;
	x.expo+=del*2;
	x.simplify();
	auto ans=float2048<siz+2>(x)/float2048<siz+2>(2);
	while(true)
	{
		cnt++;
		float2048 tmp=ans;
		ans=(ans+x/ans)/float2048<siz+2>(2);
		if(ans==tmp)
			break;
	}
	ans.expo-=del;
	ans.simplify();
	return float2048<siz>(ans);
}
template<int siz> float2048<siz> cbrt(const float2048<siz> &a)
{
	if(!a.val)
		return float2048<siz>(0);
	float2048<siz+2> x(a);
	int cnt=0,del=(1-(siz+2)*M_MAX_DIGIT-x.expo)/3;
	x.expo+=del*3;
	x.simplify();
	auto ans=float2048<siz+2>(x)/float2048<siz+2>(3);
	while(true)
	{
		cnt++;
		float2048 tmp=ans;
		ans=(float2048<siz+2>(2)*ans+x/ans/ans)/float2048<siz+2>(3);
		if(ans==tmp)
			break;
	}
	ans.expo-=del;
	ans.simplify();
	return float2048<siz>(ans);
}
template<int siz> float2048<siz> calc_pi()
{
	if(LAS_!=-1)
		return float2048<siz>(VAL_,EXPO_);
	const auto f0(sqrt(float2048<siz+2>("1823176476672000")));
	const auto f1(float2048<siz+2>(1)/float2048<siz+2>("10939058860032000"));
	const float2048<siz+2> f2("545140134");
	float2048<siz+2> c1(1),c2("13591409"),ans(0);
	int cnt=0;
	while(cnt*12<=M_MAX_DIGIT*(siz+2))
	{
		ans+=c1*c2;
		cnt++;
		c1*=-float2048<siz+2>(6*cnt-5)*float2048<siz+2>(2*cnt-1)*float2048<siz+2>(6*cnt-1)*f1/float2048<siz+2>(1ll*cnt*cnt*cnt);
		c2+=f2;
	}
	ans=float2048<siz+2>(1)/ans*f0;
	LAS_=siz;
	VAL_=ans.val,EXPO_=ans.expo;
	return float2048<siz>(VAL_,EXPO_);
}
template<int siz> float2048<siz> cos(const float2048<siz> &a)
{
	const float2048<siz+2> PI=calc_pi<siz+2>();
	float2048<siz+2> x=remainder(float2048<siz+2>(a),float2048<siz+2>(2)*PI);
	if(!x.val)
		return float2048<siz>(1);
	const float2048<siz+2> f2(2),f1(1);int NUM=0;
	while(x.expo>-(siz+2)*M_MAX_DIGIT)
		x/=f2,NUM++;
	float2048<siz+2> ans(0),now(1);int cnt=0;
	while(true)
	{
		ans+=now;
		now*=-x*x;
		cnt+=2;
		now=now/float2048<siz+2>(1ll*cnt*(cnt-1));
		if(now.expo-ans.expo<-(siz+2)*M_MAX_DIGIT)
			break;
	}
	while(NUM)
		ans=f2*ans*ans-f1,NUM--;
	return float2048<siz>(ans);
}
template<int siz> float2048<siz> sin(const float2048<siz> &a)
{
	float2048<siz+2> tmp=cos(float2048<siz+2>(a));
	return float2048<siz>(sqrt(float2048<siz+2>(1)-tmp*tmp));
}
template<int siz> float2048<siz> tan(const float2048<siz> &a)
{
	float2048<siz+2> tmp=cos(float2048<siz+2>(a));
	if(!tmp.val)
		throw std::domain_error("Beyond the domain of definition!");
	return float2048<siz>(sqrt(float2048<siz+2>(1)-tmp*tmp)/tmp);
}
template<int siz> float2048<siz> atan_(const float2048<siz> &a)
{
	if(!a.val)
		return float2048<siz>(0);
	float2048<siz+2> ans(0),x(a),now(x);int cnt=1;
	while(true)
	{
		ans+=now;
		now*=-x*x;
		now*=float2048<siz+2>(cnt);
		cnt+=2;
		now/=float2048<siz+2>(cnt);
		if(now.expo-ans.expo<-(siz+2)*M_MAX_DIGIT)
			return float2048<siz>(ans);
	}
}
template<int siz> float2048<siz> atan(const float2048<siz> &a)
{
	if(!a.val)
		return float2048<siz>(0);
	const int op=a.expo>=1-siz*M_MAX_DIGIT;
	float2048<siz+2> x(abs(a));
	if(op)
		x=float2048<siz+2>(1)/x;
	int t=0;float2048<siz+2> f01("0.1"),f1(1);
	while(x.expo>=-siz*M_MAX_DIGIT)
		++t,x=(x-f01)/(f1+f01*x);
	float2048<siz+2>ans=atan_(x);
	if(t)
	{
		const float2048<siz+2> f0=atan_(f01);
		ans+=f0*float2048<siz+2>(t);
	}
	if(op)
		ans=calc_pi<siz+2>()/float2048<siz+2>(2)-ans;
	if(!a.val.sgn())
		ans=-ans;
	return float2048<siz>(ans);
}
template<int siz> float2048<siz> asin(const float2048<siz> &a)
{
	float2048<siz+2> x(a),y(sqrt(float2048<siz+2>(1)-x*x));
	if(!y.val)
		return a.val.sgn()?calc_pi<siz>()/float2048<siz>(2):-calc_pi<siz>()/float2048<siz>(2);
	return float2048<siz>(atan(x/y));
}
template<int siz> float2048<siz> acos(const float2048<siz> &a){return calc_pi<siz>()/float2048<siz>(2)-asin(a);}
template<int siz> float2048<siz> sinh(const float2048<siz> &a)
{
	float2048<siz+2> tmp=exp(float2048<siz+2>(a));
	return float2048<siz>((tmp-float2048<siz+2>(1)/tmp)/float2048<siz+2>(2));
}
template<int siz> float2048<siz> cosh(const float2048<siz> &a)
{
	float2048<siz+2> tmp=exp(float2048<siz+2>(a));
	return float2048<siz>((tmp+float2048<siz+2>(1)/tmp)/float2048<siz+2>(2));
}
template<int siz> float2048<siz> tanh(const float2048<siz> &a)
{
	float2048<siz+2> tmp=exp(float2048<siz+2>(a)),tmp_=float2048<siz+2>(1)/tmp;
	return float2048<siz>((tmp-tmp_)/(tmp+tmp_));
}
template<int siz> float2048<siz> asinh (const float2048<siz> &a)
{
	float2048<siz+2> x(a);
	return float2048<siz>(ln(x+sqrt(x*x+float2048<siz+2>(1))));
}
template<int siz> float2048<siz> acosh (const float2048<siz> &a)
{
	float2048<siz+2> x(a);
	return float2048<siz>(ln(x+sqrt(x*x-float2048<siz+2>(1))));
}
template<int siz> float2048<siz> atanh (const float2048<siz> &a)
{
	float2048<siz+2> x(a),f1(1);
	if(!(f1-x).val)
		throw std::domain_error("Beyond the domain of definition!");
	return float2048<siz>(ln((f1+x)/(f1-x))/float2048<siz+2>(2));
}

/*This is a class designed for high-precision float.
 *You can assign the precision.
 *The default precision is 180 digits.
 *And it is based on int65536
 */
template<int siz>
class float2048
{
	friend class MY_FLOAT;
	template<int sz> friend class float2048;
	friend float2048 Move_ <siz> (const float2048 &a,const int &b);
	friend float2048 operator+ <siz> (const float2048&);
	friend float2048 operator+ <siz> (const float2048&,const float2048&);
	friend float2048 operator- <siz> (const float2048&);
	friend float2048 operator- <siz> (const float2048&,const float2048&);
	friend float2048 operator* <siz> (const float2048&,const float2048&);
	friend float2048 operator/ <siz> (const float2048&,const float2048&);
	friend float2048 operator% <siz> (const float2048&,const float2048&);
	friend float2048 operator^ <siz> (const float2048&,const float2048&);
	friend std::istream& operator>> <siz> (std::istream&,float2048 &);
	friend std::ostream& operator<< <siz> (std::ostream&,const float2048 &);
	friend bool operator== <siz> (const float2048&,const float2048&);
	friend std::strong_ordering operator<=> <siz> (const float2048&,const float2048&);
	friend float2048 abs <siz> (const float2048&);
	friend float2048 floor <siz> (const float2048&);
	friend float2048 ceil <siz> (const float2048&);
	friend float2048 round <siz> (const float2048&);
	friend float2048 cround <siz> (const float2048&);
	friend float2048 cut <siz> (const float2048&);
	friend bool is_integer <siz> (const float2048&);
	friend float2048 fmod <siz> (const float2048&,const float2048&);
	friend float2048 remainder <siz> (const float2048&,const float2048&);
	template<int sz> friend float2048<sz> exp_(const float2048<sz>&);
	template<int sz> friend float2048<sz> exp(const float2048<sz>&);
	template<int sz> friend float2048<sz> ln_(const float2048<sz>&);
	template<int sz> friend float2048<sz> ln(const float2048<sz>&);
	template<int sz> friend float2048<sz> pow(const float2048<sz>&,const float2048<sz>&);
	template<int sz> friend float2048<sz> sqrt(const float2048<sz>&);
	template<int sz> friend float2048<sz> cbrt(const float2048<sz>&);
	template<int sz> friend float2048<sz> calc_pi();
	template<int sz> friend float2048<sz> cos(const float2048<sz>&);
	template<int sz> friend float2048<sz> sin(const float2048<sz>&);
	template<int sz> friend float2048<sz> tan(const float2048<sz>&);
	template<int sz> friend float2048<sz> atan_(const float2048<sz>&);
	template<int sz> friend float2048<sz> sinh(const float2048<sz>&);
	template<int sz> friend float2048<sz> cosh(const float2048<sz>&);
	template<int sz> friend float2048<sz> tanh(const float2048<sz>&);
	template<int sz> friend float2048<sz> asinh(const float2048<sz>&);
	template<int sz> friend float2048<sz> acosh(const float2048<sz>&);
	template<int sz> friend float2048<sz> atanh(const float2048<sz>&);
private:
	int65536 val;
	int expo=0;
public:
	void simplify();
	float2048()=default;
	explicit float2048(const int &a):val(int65536(a)){simplify();}
	explicit float2048(const long long &a):val(int65536(a)){simplify();}
	explicit float2048(const std::string &a);
	explicit float2048(int65536 a,const int &b=0):val(std::move(a)),expo(b){simplify();}
	template<int sz> explicit float2048(const float2048<sz> &a){val=a.val;expo=a.expo;simplify();}
	[[nodiscard]] int sgn()const{return val.sgn();}
	[[nodiscard]] int at(const int &a)const{return val.at(a);}
	[[nodiscard]] int logb10()const{return val?expo+siz*M_MAX_DIGIT-1:0;}
	template<int sz> float2048& operator=(const float2048<sz> &a){*this=float2048(a);return *this;}
	float2048& operator+=(const float2048 &a){return *this=*this+a;}
	float2048& operator-=(const float2048 &a){return *this=*this-a;}
	float2048& operator*=(const float2048 &a){return *this=*this*a;}
	float2048& operator/=(const float2048 &a){return *this=*this/a;}
	float2048& operator%=(const float2048 &a){return *this=*this%a;}
	float2048& operator^=(const float2048 &a){return *this=*this^a;}
	float2048& operator++(){return *this+=float2048(1);}
	float2048 operator++(int){float2048 tmp=*this;++*this;return tmp;}
	float2048& operator--(){return *this-=float2048(1);}
	float2048 operator--(int){float2048 tmp=*this;--*this;return tmp;}
	explicit operator int()const{return static_cast<int>(Move_(val,expo));}
	explicit operator long long()const{return static_cast<long long>(Move_(val,expo));}
	explicit operator bool()const{return static_cast<bool>(val);}
	explicit operator int65536()const{return Move_(val,expo);}
	explicit operator std::string()const{return std::string(val)+"e"+std::to_string(expo);}
};

template<int siz> void float2048<siz>::simplify()
{
	const int d=Count_Digit_(val);
	if(!d)
		expo=1-siz*M_MAX_DIGIT;
	else if(d>siz*9)
		expo+=d-siz*9,val=Right_Move_(val,d-siz*9);
	else if(d<siz*9)
		expo+=d-siz*9,val=Left_Move_(val,siz*9-d);
}
template<int siz> float2048<siz>::float2048(const std::string &a)
{
	int pos_d=-1,pos_e=-1;
	for(int i=0;i<static_cast<int>(a.size());i++)
		if(a[i]=='.')
		{
			if(pos_d!=-1)
			{
#ifdef INTELLIGENT_H
				throw invalid_expression("Illegal symbol!");
#endif
				return ;
			}
			pos_d=i;
		}
		else if(a[i]=='e')
		{
			if(pos_e!=-1)
			{
#ifdef INTELLIGENT_H
				throw invalid_expression("Illegal symbol!");
#endif
				return ;
			}
			pos_e=i;
		}
	if(pos_d!=-1&&pos_e!=-1)
	{
		if(pos_d>pos_e)
		{
#ifdef INTELLIGENT_H
			throw invalid_expression("Illegal symbol!");
#endif
			return ;
		}
		val=int65536(a.substr(0,pos_d)+a.substr(pos_d+1,pos_e-pos_d-1));
		int st_pos=pos_e;
		if(st_pos+1!=a.size()&&a[st_pos+1]=='-')
			st_pos++;
		for(int i=st_pos+1;i<static_cast<int>(a.size());i++)
			if(a[i]>='0'&&a[i]<='9')
				expo=expo*10+a[i]-'0';
			else
			{
#ifdef INTELLIGENT_H
				throw invalid_expression("Illegal symbol!");
#endif
				return ;
			}
		if(a[st_pos]=='-')
			expo=-expo;
		expo-=pos_e-pos_d-1;
	}
	if(pos_e==0||pos_e+1==static_cast<int>(a.size()))
		throw invalid_expression("Illegal symbol!");
	if(pos_d!=-1)
	{
		val=int65536(a.substr(0,pos_d)+a.substr(pos_d+1,a.size()-pos_d-1));
		expo=pos_d+1-static_cast<int>(a.size());
	}
	else if(pos_e==-1)
		val=int65536(a);
	else
	{
		val=int65536(a.substr(0,pos_e));
		if(pos_e+1!=a.size()&&(a[pos_e+1]=='-'||a[pos_e+1]=='+'))
			pos_e++;
		for(int i=pos_e+1;i<static_cast<int>(a.size());i++)
			if(a[i]>='0'&&a[i]<='9')
				expo=expo*10+a[i]-'0';
			else
			{
#ifdef INTELLIGENT_H
				throw invalid_expression("Illegal symbol!");
#endif
				return ;
			}
		if(a[pos_e]=='-')
			expo=-expo;
	}
	simplify();
}

#endif