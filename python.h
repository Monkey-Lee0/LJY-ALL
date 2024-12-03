#ifndef PYTHON_H
#define PYTHON_H

#include "intelligent.h"
#include "float2048.h"
#include <unordered_map>
#include <any>
#include <utility>

class Tuple
{
    friend Tuple operator+(const Tuple&,const Tuple&);
    friend Tuple operator*(const Tuple&,const int65536&);
    friend Tuple operator*(const int65536&,const Tuple&);
    friend std::strong_ordering operator<=>(const Tuple&,const Tuple&);
    friend bool operator==(const Tuple&,const Tuple&);
public:
    std::vector<std::any> val;
    Tuple()=default;
    explicit Tuple(const std::any &a){val.push_back(a);}
    [[nodiscard]] int SIZE()const{return static_cast<int>(val.size());}
    std::any operator[](const int&a)const;
    std::any operator[](const int65536&a)const;
    explicit operator bool()const{return !val.empty();}
};

inline Tuple operator*(const Tuple& a,const int65536 &b)
{
    Tuple c;
    for(int65536 i(1);i<=b;++i)
        c=c+a;
    return c;
}
inline Tuple operator*(const int65536 &a,const Tuple &b){return b*a;}
inline std::any Tuple::operator[](const int &a)const
{
    if(a<-static_cast<int>(val.size())||a>=val.size())
        throw undefined_behavior("tuple index out of range");
    if(a<-static_cast<int>(val.size()))
        return val[a+val.size()];
    return val[a];
}
inline std::any Tuple::operator[](const int65536 &a)const{return val[static_cast<int>(a)];}

class str
{
    friend str operator+(const str&,const str&);
    friend str operator*(const str&,const int65536&);
    friend str operator*(const int65536&,const str&);
    friend std::strong_ordering operator<=>(const str&,const str&);
    friend bool operator==(const str&,const str&);
private:
    std::string val;
public:
    str()=default;
    explicit str(const char a){val.push_back(a);}
    explicit str(std::string a):val(std::move(a)){}
    [[nodiscard]] int SIZE()const{return static_cast<int>(val.size());}
    str operator[](const int &a)const;
    str operator[](const int65536 &a)const;
    str operator+=(const str &a){return *this=*this+a;}
    str operator*=(const int65536 &a){return *this=*this*a;}
    explicit operator bool()const{return !val.empty();}
    explicit operator std::string()const{return val;}
};

inline str operator+(const str& a,const str& b)
{
    str c=a;
    for(const auto t:b.val)
        c.val.push_back(t);
    return c;
}
inline str operator*(const str& a,const int65536 &b)
{
    str c;const int tmp=static_cast<int>(b);
    for(int i=1;i<=tmp;++i)
        c=c+a;
    return c;
}
inline str operator*(const int65536 &a,const str &b){return b*a;}
inline str str::operator[](const int &a)const
{
    if(a<-static_cast<int>(val.size())||a>=val.size())
        throw undefined_behavior("tuple index out of range");
    if(a<-static_cast<int>(val.size()))
        return str(val[a+val.size()]);
    return str(val[a]);
}
inline str str::operator[](const int65536 &a)const{return (*this)[static_cast<int>(a)];}
inline std::strong_ordering operator<=>(const str &a,const str &b)
{
    const int siz=std::min(a.SIZE(),b.SIZE());
    for(int i=0;i<siz;i++)
    {
        if(a[i].val<b[i].val)
            return std::strong_ordering::less;
        if(a[i].val>b[i].val)
            return std::strong_ordering::greater;
    }
    if(a.SIZE()>siz)
        return std::strong_ordering::greater;
    if(b.SIZE()>siz)
        return std::strong_ordering::less;
    return std::strong_ordering::equal;
}
inline bool operator==(const str &a,const str &b)
{
    if(a.SIZE()!=b.SIZE())
        return false;
    for(int i=0;i<a.SIZE();i++)
        if(a[i].val!=b[i].val)
            return false;
    return true;
}

class wrap
{
public:
    std::any val;
    wrap()=default;
    explicit wrap(std::any a):val(std::move(a)){}
};

inline int COUNT_OF_FUNCTION=0;

class python_function
{
    friend bool operator==(const python_function&,const python_function&);
public:
    std::vector<std::string> code,parameter;
    std::map<std::string,std::any> dict;
    int id=0;
    python_function()=default;
    explicit python_function(const std::string &);
    std::unordered_map<std::string, std::any> call(const std::vector<std::pair<std::string, std::any> > &);
};

inline bool operator==(const python_function &a,const python_function &b){return a.id==b.id;}

struct running_information
{
    int indentation_count,pre_state,pre_pos;
    /*pre_state:
     *0 normal
     *1 successful 'if'
     *2 failed 'if'
     *3 successful 'elif'
     *4 failed 'elif'
     *5 successful 'while'
     *6 inside the definition of a function
     */
    running_information(const int &a,const int &b,const int &c):indentation_count(a),pre_state(b),pre_pos(c){}
};





inline std::unordered_map<std::string,std::any> variable;

inline std::unordered_map<std::string,std::any> *Dict=&variable;

inline std::any *interpreter_variable(const std::string &var,std::unordered_map<std::string,std::any> *dict)
{
    if(!is_variable(var))
        throw invalid_expression("Invalid expression!");
    if(!dict->contains(var))
        throw undefined_behavior("Undefined variable \'"+std::any_cast<std::string>(var)+'\'');
    return &(*dict)[var];
}

inline std::any interpreter_value(const std::any &var)
{
    if(var.type()!=typeid(std::string))
        return var;
    try
    {
        if(!is_variable(std::any_cast<std::string>(var)))
            throw invalid_expression("unknown object "+std::any_cast<std::string>(var));
        return *interpreter_variable(std::any_cast<std::string>(var),Dict);
    }
    catch(...)
    {
        return *interpreter_variable(std::any_cast<std::string>(var),&variable);
    }
}

inline Tuple operator+(const Tuple& a,const Tuple& b)
{
    Tuple c=a;
    for(const auto& t:b.val)
        c.val.push_back(interpreter_value(t));
    return c;
}

inline std::unordered_map<std::string, std::any> python_function::call(const std::vector<std::pair<std::string, std::any> > &par)
{
    std::unordered_map<std::string,std::any> _dict;
    if(static_cast<int>(par.size())>parameter.size())
        throw invalid_expression("too many parameters");
    int cnt=0;
    for(int i=0;i<static_cast<int>(par.size());i++)
        if(par[i].first.empty())
        {
            if(cnt)
                throw invalid_expression("positional argument follows keyword argument");
            _dict[parameter[i]]=interpreter_value(par[i].second);
        }
        else
        {
            cnt=1;
            if(_dict.contains(par[i].first))
                throw invalid_expression("multiple values for the same argument");
            _dict[par[i].first]=interpreter_value(par[i].second);
        }
    for(const auto & i :parameter)
        if(!_dict.contains(i))
        {
            if(dict[i].type()==typeid(std::string))
                throw invalid_expression("missing argument");
            _dict[i]=dict[i];
        }
    if(_dict.size()>parameter.size())
        throw invalid_expression("invalid keyword argument");
    return _dict;
}

inline std::string type_name(const std::any &aa)
{
    const std::any a=interpreter_value(aa);
    if(a.type()==typeid(int))
        return "symbol";
    if(a.type()==typeid(python_function))
        return "function";
    if(a.type()==typeid(int65536))
        return "int";
    if(a.type()==typeid(float2048<>))
        return "float";
    if(a.type()==typeid(bool))
        return "bool";
    if(a.type()==typeid(str))
        return "str";
    if(a.type()==typeid(Tuple))
        return "tuple";
    if(a.type()==typeid(wrap))
        return "wrap";
    if(a.type()==typeid(std::string))
        return "undefined";
    if(a.type()==typeid(std::vector<std::pair<std::string,std::any>>))
        return "temp";
    if(a.type()==typeid(long long))
        return "none";
    return "";
}

inline float2048<> cast_to_float(const std::any &aa)
{
    std::any a=interpreter_value(aa);
    if(a.type()==typeid(int65536))
        return float2048(std::any_cast<int65536>(a));
    if(a.type()==typeid(bool))
        return float2048(std::any_cast<bool>(a)?1:0);
    if(a.type()==typeid(str))
        return float2048(static_cast<std::string>(std::any_cast<str>(a)));
    if(a.type()==typeid(Tuple))
        throw undefined_behavior("can't convert from 'tuple' to 'float'");
    if(a.type()==typeid(python_function))
        throw undefined_behavior("can't convert from 'python_function' to 'float'");
    if(a.type()==typeid(long long))
        throw undefined_behavior("can't convert from 'none' to 'float'");
    return std::any_cast<float2048<>>(a);
}

inline int65536 cast_to_int(const std::any &aa)
{
    std::any a=interpreter_value(aa);
    if(a.type()==typeid(float2048<>))
        return int65536(std::any_cast<float2048<>>(a));
    if(a.type()==typeid(bool))
        return int65536(std::any_cast<bool>(a)?1:0);
    if(a.type()==typeid(str))
        return int65536(static_cast<std::string>(std::any_cast<str>(a)));
    if(a.type()==typeid(Tuple))
        throw undefined_behavior("can't convert from 'tuple' to 'int'");
    if(a.type()==typeid(python_function))
        throw undefined_behavior("can't convert from 'python_function' to 'int'");
    if(a.type()==typeid(long long))
        throw undefined_behavior("can't convert from 'none' to 'float'");
    return std::any_cast<int65536>(a);
}

inline bool cast_to_bool(const std::any &aa)
{
    std::any a=interpreter_value(aa);
    if(a.type()==typeid(float2048<>))
        return static_cast<bool>(std::any_cast<float2048<>>(a));
    if(a.type()==typeid(int65536))
        return static_cast<bool>(std::any_cast<int65536>(a));
    if(a.type()==typeid(str))
        return !static_cast<std::string>(std::any_cast<str>(a)).empty();
    if(a.type()==typeid(Tuple))
        return static_cast<bool>(std::any_cast<Tuple>(a));
    if(a.type()==typeid(python_function))
        return true;
    if(a.type()==typeid(long long))
        return false;
    return std::any_cast<bool>(a);
}

inline str cast_to_str(const std::any &aa)
{
    std::any a=interpreter_value(aa);
    if(a.type()==typeid(int65536))
        return str(std::string(std::any_cast<int65536>(a)));
    if(a.type()==typeid(float2048<>))
    {
        std::stringstream ss;
        std::streambuf* buffer=std::cout.rdbuf();
        std::cout.rdbuf(ss.rdbuf());
		std::cout<<std::fixed<<std::setprecision(6)<<std::any_cast<float2048<>>(a);
//        std::cout<<std::setprecision(12)<<std::any_cast<float2048<>>(a);
        const std::string res(ss.str());
        std::cout.rdbuf(buffer);
        return str(res);
    }
    if(a.type()==typeid(bool))
        return str(std::any_cast<bool>(a)?"True":"False");
    if(a.type()==typeid(Tuple))
    {
        const auto tpl=std::any_cast<Tuple>(a);
        str res;
        res+=str("(");
        if(tpl.SIZE())
        {
            res+=cast_to_str(tpl[0]);
            for(int i=1;i<tpl.SIZE();i++)
                res+=str(", "),res+=cast_to_str(tpl[i]);
            if(tpl.SIZE()==1)
                res+=str(",");
        }
        res+=str(")");
        return res;
    }
    if(a.type()==typeid(int))
        return str("operator"+std::to_string(std::any_cast<int>(a)));
    if(a.type()==typeid(python_function))
        return str("python_function"+std::to_string(std::any_cast<python_function>(a).id));
    if(a.type()==typeid(long long))
        return str("None");
    return std::any_cast<str>(a);
}

inline Tuple cast_to_tuple(const std::any &aa)
{

    std::any a=interpreter_value(aa);
    if(a.type()==typeid(str))
    {
        Tuple tmp;
        auto S=std::any_cast<str>(a);
        for(int i=0;i<S.SIZE();i++)
            tmp.val.emplace_back(S[i]);
        return tmp;
    }
    if(a.type()==typeid(str))
        return std::any_cast<Tuple>(a);
    if(a.type()==typeid(int65536))
        throw undefined_behavior("can't convert from 'int' to 'tuple'");
    if(a.type()==typeid(float2048<>))
        throw undefined_behavior("can't convert from 'float' to 'tuple'");
    if(a.type()==typeid(bool))
        throw undefined_behavior("can't convert from 'bool' to 'tuple'");
    if(a.type()==typeid(python_function))
        throw undefined_behavior("can't convert from 'function' to 'tuple'");
    if(a.type()==typeid(long long))
        throw undefined_behavior("can't convert from 'none' to 'tuple'");
    return {};
}

inline void print_to_screen(const std::any &aa,const bool op=false)
{
    const std::any a=interpreter_value(aa);
    if(a.type()==typeid(int65536))
        std::cout<<std::any_cast<int65536>(a);
    if(a.type()==typeid(float2048<>))
        std::cout<<std::fixed<<std::setprecision(6)<<std::any_cast<float2048<>>(a);
        // std::cout<<std::setprecision(12)<<std::any_cast<float2048<>>(a);
    if(a.type()==typeid(bool))
        std::cout<<(std::any_cast<bool>(a)?"True":"False");
    if(a.type()==typeid(Tuple))
    {
        const auto tpl=std::any_cast<Tuple>(a);
        std::cout<<"(";
        if(tpl.SIZE())
        {
            print_to_screen(tpl[0]);
            for(int i=1;i<tpl.SIZE();i++)
                std::cout<<", ",print_to_screen(tpl[i]);
            if(tpl.SIZE()==1)
                std::cout<<",";
        }
        std::cout<<")";
    }
    if(a.type()==typeid(int))
        std::cout<<"operator"<<std::any_cast<int>(a);
    if(a.type()==typeid(python_function))
        std::cout<<"python_function"<<std::any_cast<python_function>(a).id;
    if(a.type()==typeid(str))
    {
        auto tmp=std::string(std::any_cast<str>(a));
        if(!op)
        {
            int op1=0,op2=0;
            for(const auto t:tmp)
                if(t=='\'')
                    op1=1;
                else if(t=='\"')
                    op2=1;
            if(!op1||op2)
                std::cout<<"\'"+tmp+"\'";
            else
                std::cout<<"\""+tmp+"\"";
        }
        else
            for(int i=0;i<static_cast<int>(tmp.size());i++)
                if(tmp[i]=='\\')
                {
                    i++;
                    if(tmp[i]=='\\')
                        std::cout<<'\\';
                    else if(tmp[i]=='0')
                        std::cout<<'\0';
                    else if(tmp[i]=='n')
                        std::cout<<'\n';
                    else if(tmp[i]=='t')
                        std::cout<<'\t';
                }
                else
                    std::cout<<tmp[i];
    }
    if(a.type()==typeid(long long))
        std::cout<<"None";
}

inline int get_priority(const std::any &a)
{
    switch(std::any_cast<int>(a))
    {
        case 11:
        case 13:
        case 53:return 0;
        case 54:return 1;
        case 61:
        case 62:
        case 63:
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 55:return 2;
        case 41:return 3;
        case 31:return 4;
        case 32:return 5;
        case 33:return 6;
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:return 7;
        case 1:
        case 2:return 8;
        case 3:
        case 4:
        case 5:
        case 6:return 9;
        case 51:
        case 52:return 10;
        case 7:return 11;
        case 12:
        case 14:return 12;
        default:return -1;
    }
}

inline bool higher_priority(const std::any &a,const std::any &b)
{
    const int tmp_a=get_priority(a),tmp_b=get_priority(b);
    if(tmp_a==2&&tmp_b==2)
        return false;
    return tmp_a>=tmp_b;
}

inline bool higher_priority_(const std::any &a,const std::any &b)
{
    const int tmp_a=get_priority(a),tmp_b=get_priority(b);
    if(tmp_a==2&&tmp_b==2)
        return false;
    return tmp_a>tmp_b;
}

inline int get_indentation(const std::string &a)
{
    for(int i=0;i<static_cast<int>(a.size());i++)
        // if(a[i]=='\t')
        //     throw indentation_error("prohibited use of tabs as indentation");
        if(a[i]!=' '&&a[i]!='\t')
            return i;
    return 0;
}

inline std::string remove_back_blank(const std::string &a)
{
    auto b=a;
    while(!b.empty()&&(b.back()=='\t'||b.back()==' '))
        b.pop_back();
    return b;
}

inline std::string remove_blank(const std::string &a)
{
    int pos=0;
    while(pos!=a.size()&&(a[pos]==' '||a[pos]=='\t'))
        pos++;
    auto b=a.substr(pos);
    while(!b.empty()&&(b.back()=='\t'||b.back()==' '))
        b.pop_back();
    return b;

}

inline int search_first_target(const std::string &a,const char &t)
{
    char now=0;
    int state=0;
    for(int i=0;i<static_cast<int>(a.size());i++)
    {
        if(state)
            state=0;
        else
        {
            if(now)
            {
                if(now==a[i])
                    now=0;
                if(a[i]=='\\')
                    state=1;
            }
            else
            {
                if(a[i]==t)
                    return i;
                if(a[i]=='\''||a[i]=='\"')
                    now=a[i];
            }
        }
    }
    return -1;
}





inline std::any interpreter_literal(const std::string &con)
{
    if(con=="True")
        return true;
    if(con=="False")
        return false;
    if(con=="None")
        return 0ll;
    int type=0;
    for(const auto t:con)
        if(t=='.'||t=='e')
            type=1;
    if(!con.empty()&&(con.front()=='\''||con.front()=='\"'))
        type=2;
    try
    {
        if(type==0)
            return int65536(con);
        if(type==1)
            return float2048<>(con);
        else
            return str(con.substr(1,con.size()-2));
    }
    catch(...)
    {
        throw invalid_expression("invalid_expression");
    }
}

inline std::any interpreter_symbol(const std::string &con)
{
    if(con=="+")
        return 1;
    if(con=="-")
        return 2;
    if(con=="*")
        return 3;
    if(con=="/")
        return 4;
    if(con=="%")
        return 5;
    if(con=="//")
        return 6;
    if(con=="**")
        return 7;

    if(con=="(")
        return 11;
    if(con==")")
        return 12;
    if(con=="[")
        return 13;
    if(con=="]")
        return 14;

    if(con=="<")
        return 21;
    if(con=="<=")
        return 22;
    if(con==">")
        return 23;
    if(con==">=")
        return 24;
    if(con=="==")
        return 25;
    if(con=="!=")
        return 26;

    if(con=="or")
        return 31;
    if(con=="and")
        return 32;
    if(con=="not")
        return 33;

    if(con==",")
        return 41;

    //51-60 for some designated operator
    //plus operator 51
    //minus operator 52
    //python_function calling operator 53
    //departure operator 54
    //allocator 55

    if(con=="=")
        return 61;
    if(con=="+=")
        return 62;
    if(con=="-=")
        return 63;
    if(con=="*=")
        return 64;
    if(con=="/=")
        return 65;
    if(con=="//=")
        return 66;
    if(con=="%=")
        return 67;
    if(con=="**=")
        return 68;

    return 0;
}

inline std::any interpreter_object(const std::string &s)
{
    if(const int tmp=std::any_cast<int>(interpreter_symbol(s)))
        return tmp;
    if(s.empty()||!is_legal_(s.front())||s=="True"||s=="False"||s=="None")
        return interpreter_literal(s);
    return s;
}

namespace operator_
{
    inline std::any operator_add(const std::any &aa,const std::any &bb)
{
    const std::any a=interpreter_value(aa),b=interpreter_value(bb);
    if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
        throw undefined_behavior(R"(unsupported operand type(s) for +: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    if(a.type()==typeid(str)||b.type()==typeid(str))
    {
        if(a.type()!=typeid(str)||b.type()!=typeid(str))
            throw undefined_behavior(R"(unsupported operand type(s) for +: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        return std::any_cast<str>(a)+std::any_cast<str>(b);
    }
    if(a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
    {
        if(a.type()!=typeid(Tuple)||b.type()!=typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for +: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        return std::any_cast<Tuple>(a)+std::any_cast<Tuple>(b);
    }
    if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
        return cast_to_float(a)+cast_to_float(b);
    return cast_to_int(a)+cast_to_int(b);
}

    inline std::any operator_plus(const std::any &aa)
    {
        const std::any a=interpreter_value(aa);
        if(a.type()==typeid(str)||a.type()==typeid(Tuple)||a.type()==typeid(python_function)||a.type()==typeid(long long))
            throw undefined_behavior(R"(bad operand type for unary +: ')"+type_name(a)+R"(')");
        if(a.type()==typeid(float2048<>))
            return cast_to_float(a);
        return cast_to_int(a);
    }

    inline std::any operator_sub(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for -: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for -: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)-cast_to_float(b);
        return cast_to_int(a)-cast_to_int(b);
    }

    inline std::any operator_minus(const std::any &aa)
    {
        const std::any a=interpreter_value(aa);
        if(a.type()==typeid(str)||a.type()==typeid(Tuple)||a.type()==typeid(python_function)||a.type()==typeid(long long))
            throw undefined_behavior(R"(bad operand type for unary -: ')"+type_name(a)+R"(')");
        if(a.type()==typeid(float2048<>))
            return -cast_to_float(a);
        return -cast_to_int(a);
    }

    inline std::any operator_mul(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for *: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str))
        {
            if(b.type()!=typeid(int65536)&&b.type()!=typeid(bool))
                throw undefined_behavior(R"(unsupported operand type(s) for *: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return any_cast<str>(a)*cast_to_int(b);
        }
        if(b.type()==typeid(str))
        {
            if(a.type()!=typeid(int65536)&&a.type()!=typeid(bool))
                throw undefined_behavior(R"(unsupported operand type(s) for *: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return any_cast<str>(b)*cast_to_int(a);
        }
        if(a.type()==typeid(Tuple))
        {
            if(b.type()!=typeid(int65536)&&b.type()!=typeid(bool))
                throw undefined_behavior(R"(unsupported operand type(s) for *: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return any_cast<Tuple>(a)*cast_to_int(b);
        }
        if(b.type()==typeid(Tuple))
        {
            if(a.type()!=typeid(int65536)&&a.type()!=typeid(bool))
                throw undefined_behavior(R"(unsupported operand type(s) for *: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return any_cast<Tuple>(b)*cast_to_int(a);
        }
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)*cast_to_float(b);
        return cast_to_int(a)*cast_to_int(b);
    }

    inline std::any operator_div(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for /: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for /: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        return cast_to_float(a)/cast_to_float(b);
    }

    inline std::any operator_div_(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for //: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for //: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return floor(cast_to_float(operator_div(a,b)));
        return cast_to_int(a)/cast_to_int(b);
    }

    inline std::any operator_mod(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for %: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for %: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)%cast_to_float(b);
        return cast_to_int(a)%cast_to_int(b);
    }

    inline std::any operator_pow(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for ** or pow(): ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for ** or pow(): ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>)||cast_to_float(b)<float2048(0))
            return cast_to_float(a)^cast_to_float(b);
        return cast_to_int(a)^cast_to_int(b);
    }

    inline std::any operator_less(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(std::string))
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for <: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
        {
            if(a.type()!=typeid(Tuple)||b.type()!=typeid(Tuple))
                throw undefined_behavior(R"(unsupported operand type(s) for <: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return std::any_cast<Tuple>(a)<std::any_cast<Tuple>(b);
        }
        if(a.type()==typeid(str)||b.type()==typeid(str))
        {
            if(a.type()!=typeid(str)||b.type()!=typeid(str))
                throw undefined_behavior(R"(unsupported operand type(s) for <: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return std::any_cast<str>(a)<std::any_cast<str>(b);
        }
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)<cast_to_float(b);
        return cast_to_int(a)<cast_to_int(b);
    }

    inline std::any operator_less_eq(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for <=: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
        {
            if(a.type()!=typeid(Tuple)||b.type()!=typeid(Tuple))
                throw undefined_behavior(R"(unsupported operand type(s) for <=: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return std::any_cast<Tuple>(a)<=std::any_cast<Tuple>(b);
        }
        if(a.type()==typeid(str)||b.type()==typeid(str))
        {
            if(a.type()!=typeid(str)||b.type()!=typeid(str))
                throw undefined_behavior(R"(unsupported operand type(s) for <=: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return std::any_cast<str>(a)<=std::any_cast<str>(b);
        }
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)<=cast_to_float(b);
        return cast_to_int(a)<=cast_to_int(b);
    }

    inline std::any operator_greater(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for >: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
        {
            if(a.type()!=typeid(Tuple)||b.type()!=typeid(Tuple))
                throw undefined_behavior(R"(unsupported operand type(s) for >: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return std::any_cast<Tuple>(a)>std::any_cast<Tuple>(b);
        }
        if(a.type()==typeid(str)||b.type()==typeid(str))
        {
            if(a.type()!=typeid(str)||b.type()!=typeid(str))
                throw undefined_behavior(R"(unsupported operand type(s) for >: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return std::any_cast<str>(a)>std::any_cast<str>(b);
        }
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)>cast_to_float(b);
        return cast_to_int(a)>cast_to_int(b);
    }

    inline std::any operator_greater_eq(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for >=: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
        {
            if(a.type()!=typeid(Tuple)||b.type()!=typeid(Tuple))
                throw undefined_behavior(R"(unsupported operand type(s) for >=: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return std::any_cast<Tuple>(a)>=std::any_cast<Tuple>(b);
        }
        if(a.type()==typeid(str)||b.type()==typeid(str))
        {
            if(a.type()!=typeid(str)||b.type()!=typeid(str))
                throw undefined_behavior(R"(unsupported operand type(s) for >=: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            return std::any_cast<str>(a)>=std::any_cast<str>(b);
        }
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)>=cast_to_float(b);
        return cast_to_int(a)>=cast_to_int(b);
    }

    inline std::any operator_equal(const std::any &aa,const std::any &bb)
    {
        const std::any a=interpreter_value(aa),b=interpreter_value(bb);
        if((a.type()!=typeid(int65536)&&a.type()!=typeid(float2048<>)&&a.type()!=typeid(bool))||
            (b.type()!=typeid(int65536)&&b.type()!=typeid(float2048<>)&&b.type()!=typeid(bool)))
        {
            if(a.type()!=b.type())
                return false;
            if(a.type()==typeid(python_function))
                return std::any_cast<python_function>(a)==std::any_cast<python_function>(b);
            if(a.type()==typeid(Tuple))
                return std::any_cast<Tuple>(a)==std::any_cast<Tuple>(b);
            if(a.type()==typeid(str))
                return std::any_cast<str>(a)==std::any_cast<str>(b);
            if(a.type()==typeid(long long))
                return true;
        }
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)==cast_to_float(b);
        return cast_to_int(a)==cast_to_int(b);
    }

    inline std::any operator_not_equal(const std::any &a,const std::any &b)
    {
        return !std::any_cast<bool>(operator_equal(a,b));
    }

    inline std::any operator_or(const std::any &a,const std::any &b)
    {
        if(cast_to_bool(a))
            return a;
        return b;
    }

    inline std::any operator_and(const std::any &a,const std::any &b)
    {
        if(!cast_to_bool(a))
            return a;
        return b;
    }

    inline std::any operator_not(const std::any &a){return !cast_to_bool(a);}
}
using namespace operator_;

inline std::strong_ordering operator<=>(const Tuple &a, const Tuple &b)
{
    const int siz=std::min(a.SIZE(),b.SIZE());
    for(int i=0;i<siz;i++)
    {
        if(std::any_cast<bool>(operator_less(a[i],b[i])))
            return std::strong_ordering::less;
        if(std::any_cast<bool>(operator_greater(a[i],b[i])))
            return std::strong_ordering::greater;
    }
    if(a.SIZE()>siz)
        return std::strong_ordering::greater;
    if(b.SIZE()>siz)
        return std::strong_ordering::less;
    return std::strong_ordering::equal;
}
inline bool operator==(const Tuple &a,const Tuple &b)
{
    if(a.SIZE()!=b.SIZE())
        return false;
    for(int i=0;i<a.SIZE();i++)
        if(std::any_cast<bool>(operator_not_equal(a[i],b[i])))
            return false;
    return true;
}

inline std::any operator_index(const std::any &aa,const std::any &bb)
{
    const std::any a=interpreter_value(aa),b=interpreter_value(bb);
    if(b.type()!=typeid(int65536)&&b.type()!=typeid(bool))
        throw undefined_behavior(R"(unsupported operand type(s) for []: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    const int tmp=static_cast<int>(cast_to_int(b));
    if(a.type()==typeid(str))
        return std::any_cast<str>(a)[tmp];
    if(a.type()==typeid(Tuple))
        return std::any_cast<Tuple>(a)[tmp];
    throw undefined_behavior(R"(unsupported operand type(s) for []: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
}

inline std::any operator_assignment(const std::any &a,const std::any &bb)
{
    std::any b=interpreter_value(bb);
    if(b.type()==typeid(wrap))
        b=std::any_cast<wrap>(b).val;
    if(a.type()==typeid(std::string))
    {
        // Why we can assign to global variable without keyword "global"?
        if(variable.contains(std::any_cast<std::string>(a)))
            variable[std::any_cast<std::string>(a)]=b;
        else
            (*Dict)[std::any_cast<std::string>(a)]=b;
    }
    else
    {
        if(a.type()!=typeid(Tuple))
            throw invalid_expression("cannot assign to literal");
        if(b.type()!=typeid(Tuple))
            throw undefined_behavior("cannot unpack");
        const auto pa=std::any_cast<Tuple>(a),pb=std::any_cast<Tuple>(b);
        if(pa.SIZE()!=pb.SIZE())
            throw undefined_behavior("cannot unpack");
        for(int i=0;i<pa.SIZE();i++)
            operator_assignment(pa[i],pb[i]);
    }
    return wrap(b);
}

inline std::any interpreter_block(std::vector<std::string>,int,std::unordered_map<std::string,std::any>*);

inline int CNT=0;
inline std::vector<std::any> raw_expression_;

inline void translator(const std::string &s)
{
    std::string las,aux;
    int state=-1,shift=0;
    char pre=0;
    for(int i=0;i<static_cast<int>(s.size());i++)
    {
        char t=s[i];
        if(state==-1)// blank mode
        {
            if(is_legal(t)||t=='.')
                state=0,las=t;
            else if(is_symbol(t))
                state=1,las=t;
            else if(t=='\"')
                state=2,las=t;
            else if(t=='\'')
                state=3,las=t;
            else if(is_blank(t))
                state=-1;
            else if(t=='#')
                break;
            else
                las.clear(),state=-1,raw_expression_.push_back(0ll);
        }
        else if(state==0)// non-string literal mode
        {
            if(is_legal(t)||t=='.')
                state=0,las+=t;
            else if(is_symbol(t))
                state=1,raw_expression_.push_back(interpreter_object(las)),las=t;
            else if(t=='\''||t=='\"')
            {
                if(las=="f")
                {
                    las=t;
                    if(t=='\"')
                        state=5;
                    else
                        state=6;
                }
                else
                    throw invalid_expression("invalid expression");
            }
            else if(is_blank(t))
                state=-1,raw_expression_.push_back(interpreter_object(las)),las.clear();
            else if(t=='#')
            {
                state=-1;
                raw_expression_.push_back(interpreter_object(las));
                break;
            }
            else
                las.clear(),state=-1,raw_expression_.push_back(0ll);
        }
        else if(state==1)// operator mode
        {
            if(las=="("||las=="["||las=="{")
                CNT++;
            if(las==")"||las=="]"||las=="}")
                CNT--;
            if(CNT<0)
                throw invalid_expression("unmatched \'"+las+"\'");
            if(is_legal(t)||t=='.')
                state=0,raw_expression_.push_back(interpreter_object(las)),las=t;
            else if(is_symbol(t))
            {
                if((t=='='&&(las=="="||las=="<"||las==">"||las=="!"||las=="+"||las=="-"||las=="*"||las=="/"||las=="//"||las=="%"||las=="**"))
                    ||(las=="*"&&t=='*')||(las=="/"&&t=='/'))
                    state=1,las+=t;
                else
                    state=1,raw_expression_.push_back(interpreter_object(las)),las=t;
            }
            else if(t=='\"')
                state=2,raw_expression_.push_back(interpreter_object(las)),las=t;
            else if(t=='\'')
                state=3,raw_expression_.push_back(interpreter_object(las)),las=t;
            else if(is_blank(t))
                state=-1,raw_expression_.push_back(interpreter_object(las)),las.clear();
            else if(t=='#')
            {
                state=-1;
                raw_expression_.push_back(interpreter_object(las));
                break;
            }
            else
                las.clear(),state=-1,raw_expression_.push_back(0ll);
        }
        else if(state==2)// "-string mode
        {
            if(shift)
            {
                shift=0;
                if(t!='\\'&&t!='n'&&t!='t'&&t!='0'&&t!='\''&&t!='\"')
                    las+='\\';
                las+=t;
            }
            else
            {
                las+=t;
                if(t=='\\')
                    shift=1;
                else if(t=='\"')
                    state=4;
            }
        }
        else if(state==3)// '-string mode
        {
            if(shift)
            {
                shift=0;
                if(t!='\\'&&t!='n'&&t!='t'&&t!='0'&&t!='\''&&t!='\"')
                    las+='\\';
                las+=t;
            }
            else
            {
                las+=t;
                if(t=='\\')
                    shift=1;
                else if(t=='\'')
                    state=4;
            }
        }
        else if(state==4)// none-string mode
        {
            if(is_legal(t)||t=='.')
            {
                if(t=='f'&&i+1!=s.size()&&(s[i+1]=='\''||s[i+1]=='\"'))
                {
                    i++,las.pop_back();
                    if(s[i]=='\"')
                        state=5;
                    else
                        state=6;
                }
                else
                    throw invalid_expression("invalid expression");
            }
            else if(is_symbol(t))
                state=1,raw_expression_.push_back(interpreter_object(las)),las=t;
            else if(t=='\"')
                state=2,las.pop_back();
            else if(t=='\'')
                state=3,las.pop_back();
            else if(is_blank(t))
                state=4;
            else if(t=='#')
            {
                state=-1;
                raw_expression_.push_back(interpreter_object(las));
                break;
            }
            else
                las.clear(),state=-1,raw_expression_.push_back(0ll);
        }
        else if(state==5)// f"-string mode
        {
            if(t=='{')
            {
                if(i+1!=s.size()&&s[i+1]=='{')
                    i++,las+='{';
                else
                {
                    las+="\"";
                    raw_expression_.push_back(interpreter_object(las));
                    las.clear();
                    raw_expression_.push_back(interpreter_object("+"));
                    python_function tmp("a");tmp.id=-4;
                    raw_expression_.emplace_back(tmp);
                    raw_expression_.push_back(interpreter_object("("));
                    raw_expression_.push_back(interpreter_object("("));
                    state=7;
                }
            }
            else if(t=='}')
            {
                if(i+1==s.size()||s[i+1]!='}')
                    throw invalid_expression(R"(f-string: single '}' is not allowed)");
                las+=t,i++;
            }
            else if(shift)
            {
                shift=0;
                if(t!='\\'&&t!='n'&&t!='t'&&t!='0'&&t!='\''&&t!='\"')
                    las+='\\';
                las+=t;
            }
            else
            {
                las+=t;
                if(t=='\\')
                    shift=1;
                else if(t=='\"')
                    state=4;
            }
        }
        else if(state==6)// f'-string mode
        {
            if(t=='{')
            {
                if(i+1!=s.size()&&s[i+1]=='{')
                    i++,las+='{';
                else
                {
                    las+="\'";
                    raw_expression_.push_back(interpreter_object(las));
                    las.clear();
                    raw_expression_.push_back(interpreter_object("+"));
                    python_function tmp("a");tmp.id=-4;
                    raw_expression_.emplace_back(tmp);
                    raw_expression_.push_back(interpreter_object("("));
                    raw_expression_.push_back(interpreter_object("("));
                    state=8;
                }
            }
            else if(t=='}')
            {
                if(i+1==s.size()||s[i+1]!='}')
                    throw invalid_expression(R"(f-string: single '}' is not allowed)");
                las+=t,i++;
            }
            else if(shift)
            {
                shift=0;
                if(t!='\\'&&t!='n'&&t!='t'&&t!='0'&&t!='\''&&t!='\"')
                    las+='\\';
                las+=t;
            }
            else
            {
                las+=t;
                if(t=='\\')
                    shift=1;
                else if(t=='\'')
                    state=4;
            }
        }
        else if(state==7)// f"{-string mode
        {
            if(pre)
            {
                aux+=t;
                if(shift)
                    shift=0;
                else if(t=='\\')
                    shift=1;
                else if(t==pre)
                    pre=0;
            }
            else if(t=='}')
            {
                translator(aux);
                aux.clear();
                raw_expression_.push_back(interpreter_object(")"));
                raw_expression_.push_back(interpreter_object(")"));
                raw_expression_.push_back(interpreter_object("+"));
                las="\"";
                state=5;
            }
            else
            {
                if(t=='\''||t=='\"')
                    pre=t;
                aux+=t;
            }
        }
        else if(state==8)// f'{-string mode
        {
            if(pre)
            {
                aux+=t;
                if(shift)
                    shift=0;
                else if(t=='\\')
                    shift=1;
                else if(t==pre)
                    pre=0;
            }
            else if(t=='}')
            {
                translator(aux);
                aux.clear();
                raw_expression_.push_back(interpreter_object(")"));
                raw_expression_.push_back(interpreter_object(")"));
                raw_expression_.push_back(interpreter_object("+"));
                las="\'";
                state=6;
            }
            else
            {
                if(t=='\''||t=='\"')
                    pre=t;
                aux+=t;
            }
        }
    }
    if(state==2||state==3)
        throw invalid_expression("unterminated string literal");
    if(state!=-1)
    {
        if(las=="("||las=="["||las=="{")
            CNT++;
        if(las==")"||las=="]"||las=="}")
            CNT--;
        if(CNT<0)
            throw invalid_expression("unmatched \'"+las+"\'");
        raw_expression_.push_back(interpreter_object(las));
    }
}

inline std::any interpreter_arithmetic(const std::string &s,bool mode=false)
{
    translator(s);
    if(raw_expression_.empty())
        return 0ll;
    if(CNT>0&&mode)
        return 0ll;
    std::vector<std::any> raw_expression=raw_expression_;
    raw_expression_.clear();
    std::vector<int> ls(raw_expression.size(),-1),rs(raw_expression.size(),-1),fa(raw_expression.size(),-1);
    // for(const auto& t:raw_expression)
    //     print_to_screen(t),puts("");
    int now_size=static_cast<int>(raw_expression.size());
    for(int i=0;i<now_size;i++)
    {
        if(raw_expression[i].type()==typeid(int))
        {
            if(const int tmp=std::any_cast<int>(raw_expression[i]); tmp==1||tmp==2)
            {
                if(i+1==raw_expression.size())
                    throw invalid_expression("invalid syntax");
                if(fa[i]!=-1||!i)
                    raw_expression[i]=std::any_cast<int>(raw_expression[i])+50,fa[i+1]=i,ls[i]=i+1;
                else
                {
                    int pos=i-1;
                    while(fa[pos]!=-1&&higher_priority(raw_expression[fa[pos]],raw_expression[i]))
                        pos=fa[pos];
                    if(fa[pos]!=-1)
                    {
                        if(ls[fa[pos]]==pos)
                            ls[fa[pos]]=i;
                        else
                            rs[fa[pos]]=i;
                    }
                    fa[i]=fa[pos],fa[pos]=i,ls[i]=pos,rs[i]=i+1,fa[i+1]=i;
                }
            }
            else if(tmp==3||tmp==4||tmp==5||tmp==6||tmp==7||tmp==31||tmp==32||tmp==62||tmp==63||tmp==64||tmp==65||tmp==66||tmp==67||tmp==68)
            {
                if(i+1==raw_expression.size()||!i||fa[i]!=-1)
                    throw invalid_expression("invalid syntax");
                int pos=i-1;
                while(fa[pos]!=-1&&higher_priority(raw_expression[fa[pos]],raw_expression[i]))
                    pos=fa[pos];
                if(fa[pos]!=-1)
                {
                    if(ls[fa[pos]]==pos)
                        ls[fa[pos]]=i;
                    else
                        rs[fa[pos]]=i;
                }
                fa[i]=fa[pos],fa[pos]=i,ls[i]=pos,rs[i]=i+1,fa[i+1]=i;
            }
            else if(tmp==21||tmp==22||tmp==23||tmp==24||tmp==25||tmp==26)
            {
                if(i+1==raw_expression.size()||!i||fa[i]!=-1)
                    throw invalid_expression("invalid syntax");
                int pos=i-1;
                while(fa[pos]!=-1&&higher_priority_(raw_expression[fa[pos]],raw_expression[i]))
                    pos=fa[pos];
                if(fa[pos]!=-1)
                {
                    int p=std::any_cast<int>(raw_expression[fa[pos]]);
                    if(p==21||p==22||p==23||p==24||p==25||p==26)
                    {
                        ls[i]=pos;
                        rs[i]=i+1;
                        fa[i+1]=i;
                        raw_expression.emplace_back(32);
                        int tmp_node=static_cast<int>(raw_expression.size())-1,pre_fa=fa[fa[pos]];
                        ls[tmp_node]=fa[pos],rs[tmp_node]=i;
                        fa[i]=tmp_node,fa[fa[pos]]=tmp_node;
                        fa[tmp_node]=pre_fa;
                        if(pre_fa!=-1)
                        {
                            if(ls[pre_fa]==fa[pos])
                                ls[pre_fa]=tmp_node;
                            else
                                rs[pre_fa]=tmp_node;
                        }
                        continue;
                    }
                    if(ls[fa[pos]]==pos)
                        ls[fa[pos]]=i;
                    else
                        rs[fa[pos]]=i;
                }
                fa[i]=fa[pos],fa[pos]=i,ls[i]=pos,rs[i]=i+1,fa[i+1]=i;
            }
            else if(tmp==61)
            {
                if(i+1==raw_expression.size()||!i||fa[i]!=-1)
                    throw invalid_expression("invalid syntax");
                int pos=i-1;
                while(fa[pos]!=-1&&higher_priority_(raw_expression[fa[pos]],raw_expression[i]))
                    pos=fa[pos];
                if(fa[pos]!=-1)
                {
                    const int tmp_fa=std::any_cast<int>(raw_expression[fa[pos]]);
                    if(tmp_fa==53||tmp_fa==54||tmp_fa==55)
                        raw_expression[i]=55;
                    while(fa[pos]!=-1&&higher_priority(raw_expression[fa[pos]],raw_expression[i]))
                        pos=fa[pos];
                }
                if(fa[pos]!=-1)
                {
                    if(ls[fa[pos]]==pos)
                        ls[fa[pos]]=i;
                    else
                        rs[fa[pos]]=i;
                }
                fa[i]=fa[pos],fa[pos]=i,ls[i]=pos,rs[i]=i+1,fa[i+1]=i;
            }
            else if(tmp==33)
            {
                if(i+1==raw_expression.size()||(i&&fa[i]==-1))
                    throw invalid_expression("invalid syntax");
                if(i)
                {
                    if(raw_expression[i-1].type()!=typeid(int))
                        throw invalid_expression("invalid syntax");
                    if(const int tmp_=std::any_cast<int>(raw_expression[i-1]); tmp_!=11&&tmp_!=13&&tmp_!=31&&tmp_!=32&&tmp_!=33&&tmp_!=41&&tmp_!=53)
                        throw invalid_expression("invalid syntax");
                }
                fa[i+1]=i,ls[i]=i+1;
            }
            else if(tmp==11)
            {
                if(i+1==raw_expression.size())
                    throw invalid_expression(R"('(' was never closed)");
                if(i&&fa[i]==-1)
                {
                    raw_expression[i]=53;
                    int pos=i-1;
                    if(fa[pos]!=-1)
                    {
                        if(ls[fa[pos]]==pos)
                            ls[fa[pos]]=i;
                        else if(raw_expression[i+1].type()!=typeid(int)||std::any_cast<int>(raw_expression[i+1])!=12)
                            rs[fa[pos]]=i;
                    }
                    if(raw_expression[i+1].type()!=typeid(int)||std::any_cast<int>(raw_expression[i+1])!=12)
                        rs[i]=i+1,fa[i+1]=i;
                    else
                        rs[i]=-2;
                    fa[i]=fa[pos],fa[pos]=i,ls[i]=pos;
                }
                else
                    fa[i+1]=i,ls[i]=i+1;
            }
            else if(tmp==12)
            {
                int pos=i-1;
                if(pos>=0&&raw_expression[pos].type()==typeid(int)&&std::any_cast<int>(raw_expression[pos])==11)
                {
                    if(fa[pos]!=-1)
                    {
                        if(ls[fa[pos]]==pos)
                            ls[fa[pos]]=i;
                        else
                            rs[fa[pos]]=i;
                    }
                    fa[i]=fa[pos];
                    continue;
                }
                while(pos>=0&&(raw_expression[pos].type()!=typeid(int)||(std::any_cast<int>(raw_expression[pos])!=11&&std::any_cast<int>(raw_expression[pos])!=13&&std::any_cast<int>(raw_expression[pos])!=53)))
                    pos=fa[pos];
                if(pos==-1)
                    throw invalid_expression(R"(unmatched ')')");
                if(std::any_cast<int>(raw_expression[pos])==13)
                    throw invalid_expression(R"(closing parenthesis ')' does not match opening parenthesis '[')");
                fa[i]=fa[pos],ls[i]=ls[pos],rs[i]=rs[pos];
                if(ls[i]>=0)
                    fa[ls[i]]=i;
                if(rs[i]>=0)
                    fa[rs[i]]=i;
                if(fa[pos]!=-1)
                {
                    if(ls[fa[pos]]==pos)
                        ls[fa[pos]]=i;
                    else
                        rs[fa[pos]]=i;
                }
            }
            else if(tmp==13)
            {
                if(i+1==raw_expression.size())
                    throw invalid_expression(R"('[' was never closed)");
                if(!i||fa[i]!=-1)
                    throw invalid_expression("invalid syntax");
                if(fa[i-1]!=-1)
                {
                    if(ls[fa[i-1]]==i-1)
                        ls[fa[i-1]]=i;
                    else
                        rs[fa[i-1]]=i;
                }
                fa[i]=fa[i-1],fa[i-1]=i,ls[i]=i-1,rs[i]=i+1,fa[i+1]=i;
            }
            else if(tmp==14)
            {
                int pos=i-1;
                while(pos>=0&&(raw_expression[pos].type()!=typeid(int)||(std::any_cast<int>(raw_expression[pos])!=11&&std::any_cast<int>(raw_expression[pos])!=13&&std::any_cast<int>(raw_expression[pos])!=53)))
                    pos=fa[pos];
                if(pos==-1)
                    throw invalid_expression(R"(unmatched ']')");
                if(std::any_cast<int>(raw_expression[pos])!=13)
                    throw invalid_expression(R"(closing parenthesis ']' does not match opening parenthesis '(')");
                fa[i]=fa[pos],ls[i]=ls[pos],rs[i]=rs[pos];
                if(ls[i]!=-1)
                    fa[ls[i]]=i;
                if(rs[i]!=-1)
                    fa[rs[i]]=i;
                if(fa[pos]!=-1)
                {
                    if(ls[fa[pos]]==pos)
                        ls[fa[pos]]=i;
                    else
                        rs[fa[pos]]=i;
                }
            }
            else if(tmp==41)
            {
                if(!i||fa[i]!=-1)
                    throw invalid_expression("invalid syntax");
                int pos=i-1;
                while(fa[pos]!=-1&&higher_priority(raw_expression[fa[pos]],raw_expression[i]))
                    pos=fa[pos];
                if(fa[pos]!=-1)
                {
                    if(const int tmp_fa=std::any_cast<int>(raw_expression[fa[pos]]); tmp_fa==53||tmp_fa==54||tmp_fa==55)
                        raw_expression[i]=54;
                    while(fa[pos]!=-1&&higher_priority(raw_expression[fa[pos]],raw_expression[i]))
                        pos=fa[pos];
                }
                fa[i]=fa[pos];
                if(fa[pos]!=-1)
                {
                    if(ls[fa[pos]]==pos)
                        ls[fa[pos]]=i;
                    else
                        rs[fa[pos]]=i;
                }
                fa[pos]=i,ls[i]=pos;
                if(i+1!=raw_expression.size()&&(raw_expression[i+1].type()!=typeid(int)||(std::any_cast<int>(raw_expression[i+1])!=12&&std::any_cast<int>(raw_expression[i+1])!=14&&std::any_cast<int>(raw_expression[i+1])!=53)))
                    rs[i]=i+1,fa[i+1]=i;
            }
        }
        else if(i&&fa[i]==-1)
            throw invalid_expression("invalid syntax");
        else if(typeid(raw_expression_[i])==typeid(long long))
            throw invalid_expression("illegal symbol");
    }
    int now=static_cast<int>(raw_expression.size())-1;
    while(now!=-1)
    {
        if(raw_expression[now].type()==typeid(int)&&(std::any_cast<int>(raw_expression[now])==11||std::any_cast<int>(raw_expression[now])==53))
            throw invalid_expression(R"('(' was never closed)");
        if(raw_expression[now].type()==typeid(int)&&std::any_cast<int>(raw_expression[now])==13)
            throw invalid_expression(R"('[' was never closed)");
        now=fa[now];
    }
    for(int i=0,cnt=0;i<static_cast<int>(raw_expression.size());i++)
        if(raw_expression[i].type()==typeid(int))
        {
            if(int tmp=std::any_cast<int>(raw_expression[i]); tmp>=61&&tmp<=68&&cnt)
                throw invalid_expression("invalid syntax");
            else if(tmp==11)
                cnt++;
            else if(tmp==12)
                cnt--;
        }
    for(int chk=0; auto & t:raw_expression)
        if(t.type()==typeid(int))
        {
            const int tmp=std::any_cast<int>(t);
            if(tmp>=61&&tmp<=68&&chk==-1)
                throw invalid_expression("invalid syntax");
            if(tmp!=61&&tmp!=41&&tmp!=11&&tmp!=12)
                chk=-1;
        }
    // for(int i=0;i<static_cast<int>(raw_expression.size());i++)
    //     if(raw_expression[i].type()!=typeid(int)||(std::any_cast<int>(raw_expression[i])!=11&&std::any_cast<int>(raw_expression[i])!=13))
    //     {
    //         std::cerr<<i<<" "<<ls[i]<<" "<<rs[i]<<" "<<fa[i];
    //         if(raw_expression[i].type()==typeid(int))
    //             std::cerr<<" "<<std::any_cast<int>(raw_expression[i]);
    //         std::cerr<<std::endl;
    //     }
    int rt=0;
    for(int i=0;i<static_cast<int>(raw_expression.size());i++)
        if(raw_expression[i].type()!=typeid(int)||(std::any_cast<int>(raw_expression[i])!=11
            &&std::any_cast<int>(raw_expression[i])!=13&&std::any_cast<int>(raw_expression[i])!=53))
            if(fa[i]==-1)
                rt=i;
    std::vector<std::any> result;result.resize(raw_expression.size());
    std::vector<int> stack;
    stack.push_back(rt);
    while(!stack.empty())
    {
        if(int x=stack.back(); ls[x]<0&&rs[x]<0)
        {
            stack.pop_back();
            if(raw_expression[x].type()==typeid(int)&&std::any_cast<int>(raw_expression[x])==12)
                result[x]=Tuple();
            else
                result[x]=raw_expression[x];
        }
        else if(ls[x]>=0&&fa[ls[x]]==x)
            stack.push_back(ls[x]),fa[ls[x]]=-1;
        else if(rs[x]>=0&&fa[rs[x]]==x)
            stack.push_back(rs[x]),fa[rs[x]]=-1;
        else
        {
            stack.pop_back();
            if(const int tmp=std::any_cast<int>(raw_expression[x]); tmp==1)
                result[x]=operator_add(result[ls[x]],result[rs[x]]);
            else if(tmp==2)
                result[x]=operator_sub(result[ls[x]],result[rs[x]]);
            else if(tmp==3)
                result[x]=operator_mul(result[ls[x]],result[rs[x]]);
            else if(tmp==4)
                result[x]=operator_div(result[ls[x]],result[rs[x]]);
            else if(tmp==5)
                result[x]=operator_mod(result[ls[x]],result[rs[x]]);
            else if(tmp==6)
                result[x]=operator_div_(result[ls[x]],result[rs[x]]);
            else if(tmp==7)
                result[x]=operator_pow(result[ls[x]],result[rs[x]]);
            else if(tmp==21)
                result[x]=operator_less(result[ls[x]],result[rs[x]]);
            else if(tmp==22)
                result[x]=operator_less_eq(result[ls[x]],result[rs[x]]);
            else if(tmp==23)
                result[x]=operator_greater(result[ls[x]],result[rs[x]]);
            else if(tmp==24)
                result[x]=operator_greater_eq(result[ls[x]],result[rs[x]]);
            else if(tmp==25)
                result[x]=operator_equal(result[ls[x]],result[rs[x]]);
            else if(tmp==26)
                result[x]=operator_not_equal(result[ls[x]],result[rs[x]]);
            else if(tmp==31)
                result[x]=operator_or(result[ls[x]],result[rs[x]]);
            else if(tmp==32)
                result[x]=operator_and(result[ls[x]],result[rs[x]]);
            else if(tmp==33)
                result[x]=operator_not(result[ls[x]]);
            else if(tmp==51)
                result[x]=operator_plus(result[ls[x]]);
            else if(tmp==52)
                result[x]=operator_minus(result[ls[x]]);
            else if(tmp==12)
            {
                if(rs[x]==-1)
                    result[x]=result[ls[x]];
                else
                {
                    const auto val=interpreter_value(result[ls[x]]);
                    if(val.type()!=typeid(python_function))
                        throw invalid_expression("\'"+type_name(val)+R"(' object is not callable)");
                    std::vector<std::pair<std::string,std::any>> upd;
                    if(rs[x]>=0)
                    {
                        if(result[rs[x]].type()!=typeid(std::vector<std::pair<std::string,std::any>>))
                            upd.emplace_back("",result[rs[x]]);
                        else
                            upd=std::any_cast<std::vector<std::pair<std::string,std::any>>>(result[rs[x]]);
                    }
                    auto func=std::any_cast<python_function>(val);
                    if(func.id<0)
                    {
                        if(func.id>-6)
                        {
                            if(upd.empty())
                                throw invalid_expression("missing argument");
                            if(upd.size()>1)
                                throw invalid_expression("too many parameters");
                            if(!upd.front().first.empty())
                                throw invalid_expression("invalid keyword argument");
                            if(func.id==-1)
                                result[x]=cast_to_int(upd.front().second);
                            if(func.id==-2)
                                result[x]=cast_to_float(upd.front().second);
                            if(func.id==-3)
                                result[x]=cast_to_bool(upd.front().second);
                            if(func.id==-4)
                                result[x]=cast_to_str(upd.front().second);
                            if(func.id==-5)
                                result[x]=cast_to_tuple(upd.front().second);
                        }
                        else
                        {
                            for(const auto& tmp_:upd)
                                print_to_screen(tmp_.second,true),std::cout<<" ";
                            result[x]=0ll;
                            std::cout<<std::endl;
                        }
                    }
                    else
                    {
                        auto tmp_dict_=func.call(upd);
                        auto tmp_dict=Dict;
                        result[x]=interpreter_block(func.code,1,&tmp_dict_);
                        Dict=tmp_dict;
                    }
                }
            }
            else if(tmp==14)
                result[x]=operator_index(result[ls[x]],result[rs[x]]);
            else if(tmp==61)
                result[x]=operator_assignment(result[ls[x]],result[rs[x]]);
            else if(tmp==62)
                result[x]=operator_assignment(result[ls[x]],operator_add(result[ls[x]],result[rs[x]]));
            else if(tmp==63)
                result[x]=operator_assignment(result[ls[x]],operator_sub(result[ls[x]],result[rs[x]]));
            else if(tmp==64)
                result[x]=operator_assignment(result[ls[x]],operator_mul(result[ls[x]],result[rs[x]]));
            else if(tmp==65)
                result[x]=operator_assignment(result[ls[x]],operator_div(result[ls[x]],result[rs[x]]));
            else if(tmp==66)
                result[x]=operator_assignment(result[ls[x]],operator_div_(result[ls[x]],result[rs[x]]));
            else if(tmp==67)
                result[x]=operator_assignment(result[ls[x]],operator_mod(result[ls[x]],result[rs[x]]));
            else if(tmp==68)
                result[x]=operator_assignment(result[ls[x]],operator_pow(result[ls[x]],result[rs[x]]));
            else if(tmp==41)
            {
                if(raw_expression[ls[x]].type()==typeid(int)&&std::any_cast<int>(raw_expression[ls[x]])==41)
                    result[x]=result[ls[x]];
                else
                    result[x]=Tuple(result[ls[x]]);
                if(rs[x]!=-1)
                {
                    if(raw_expression[rs[x]].type()==typeid(int)&&std::any_cast<int>(raw_expression[rs[x]])==41)
                    {
                        auto tmp1=std::any_cast<Tuple>(result[x]),tmp2=std::any_cast<Tuple>(result[rs[x]]);
                        for(const auto& t:tmp2.val)
                            tmp1.val.push_back(t);
                        result[x]=tmp1;
                    }
                    else
                    {
                        auto tmp0=std::any_cast<Tuple>(result[x]);
                        tmp0.val.push_back(result[rs[x]]);
                        result[x]=tmp0;
                    }
                }
            }
            else if(tmp==54)
            {
                auto p1=result[ls[x]],p2=result[rs[x]];
                std::vector<std::pair<std::string,std::any>> res;
                if(p1.type()!=typeid(std::vector<std::pair<std::string,std::any>>))
                    res.emplace_back("",p1);
                else
                    for(auto vec=std::any_cast<std::vector<std::pair<std::string,std::any>>>(p1); const auto& t:vec)
                        res.push_back(t);
                if(p2.type()!=typeid(std::vector<std::pair<std::string,std::any>>))
                    res.emplace_back("",p2);
                else
                    for(auto vec=std::any_cast<std::vector<std::pair<std::string,std::any>>>(p2); const auto& t:vec)
                        res.push_back(t);
                result[x]=res;
            }
            else if(tmp==55)
            {
                if(raw_expression[rs[x]].type()==typeid(std::vector<std::pair<std::string,std::any>>)||raw_expression[ls[x]].type()!=typeid(std::string))
                    throw invalid_expression("invalid syntax");
                std::vector<std::pair<std::string,std::any>> res;
                res.emplace_back(std::any_cast<std::string>(raw_expression[ls[x]]),result[rs[x]]);
                result[x]=res;
            }
        }
    }
    return result[rt];
}

inline python_function::python_function(const std::string &ss):id(COUNT_OF_FUNCTION++)
{
    if(remove_blank(ss).empty())
        return ;
    int pos=-1,hav=0,cnt=0,trans=0;char sta=0;
    const std::string s=ss+",";
    for(int i=0;i<static_cast<int>(s.size());i++)
    {
        if(trans)
            trans=0;
        else if(sta)
        {
            if(s[i]=='\\')
                trans=1;
            else if(s[i]==sta)
                sta=0;
        }
        else if(s[i]=='('||s[i]=='[')
            cnt++;
        else if(s[i]==')'||s[i]==']')
            cnt=std::max(cnt-1,0);
        else if(!cnt&&s[i]==',')
        {
            int q=-1;
            for(int j=pos+1;j<i;j++)
                if(s[j]=='=')
                {
                    q=j;
                    break;
                }
            if(q==-1)
            {
                if(hav)
                    throw invalid_expression("parameter without a default follows parameter with a default");
                auto tmp=remove_blank(s.substr(pos+1,i-pos-1));
                if(!is_variable(tmp))
                    throw invalid_expression("invalid syntax");
                dict[tmp]=tmp,parameter.push_back(tmp);
            }
            else
            {
                hav=1;
                auto tmp=s.substr(pos+1,q-pos-1);
                if(!is_variable(tmp)||q==i-1)
                    throw invalid_expression("invalid syntax");
                const auto result=interpreter_arithmetic(s.substr(q+1,i-q-1));
                dict[tmp]=result,parameter.push_back(tmp);
            }
            pos=i;
        }
    }

}

inline std::any interpreter_block(std::vector<std::string> CODE_BLOCK,const int state,std::unordered_map<std::string,std::any> *dict)
{
    Dict=dict;
    std::vector<running_information> running_code;
    running_code.emplace_back(0,0,-1);
    int now=0;
    python_function now_func;
    std::string name;
    for(int i=0;i<=static_cast<int>(CODE_BLOCK.size());i++)
    {
        std::string s=(i==static_cast<int>(CODE_BLOCK.size())?"":CODE_BLOCK[i]);
        if(i!=static_cast<int>(CODE_BLOCK.size())&&s.empty())
            continue;
        const int x=get_indentation(s);
        if(running_code.back().pre_state==6&&(CNT>0||x>=running_code.back().indentation_count))
        {
            if(CNT>0)
                now_func.code.back()+=" "+s;
            else
                now_func.code.push_back(s.substr(running_code.back().indentation_count));
            translator(s);
            if(CNT==0)
                raw_expression_.clear();
            (*dict)[name]=now_func;
            continue;
        }
        if(x>now||CNT>0)
        {
            translator(s);
            if(CNT==0)
                raw_expression_.clear();
            continue;
        }
        int op=0;
        while(!running_code.empty()&&running_code.back().indentation_count!=x)
        {
            running_code.pop_back();
            if(running_code.empty())
                throw indentation_error("unexpected indent");
            if(running_code.back().pre_state==5)
            {
                i=running_code.back().pre_pos-1;
                op=1;
                break;
            }
        }
        if(op||s.empty())
            continue;
        if(x<now)
            now=x;
        if(s.size()>=x+2&&s.substr(x,2)=="if"&&(s.size()==x+2||s[x+2]==' '||s[x+2]=='\t'))
        {
            int pos=search_first_target(s,':');
            while(pos==-1)
            {
                if(i+1==CODE_BLOCK.size())
                    throw invalid_expression(R"(expected ':')");
                s+=" "+CODE_BLOCK[++i];
                pos=search_first_target(s,':');
            }
            auto following=remove_blank(s.substr(pos+1));
            int nxt;
            if(!following.empty())
            {
                for(int ii=1;ii+pos<s.size();ii++)
                    CODE_BLOCK[i].pop_back();
                if(i+1!=CODE_BLOCK.size()&&get_indentation(CODE_BLOCK[i+1])>x)
                    throw indentation_error("unexpected indent");
                std::string a;
                for(int i_=0;i_<=x;i_++)
                    a+=' ';
                nxt=x+1;
                CODE_BLOCK.insert(CODE_BLOCK.begin()+i+1,a+following);
            }
            else
            {
                if(i+1==CODE_BLOCK.size())
                    throw indentation_error(R"(expected an indented block after 'if' statement)");
                nxt=get_indentation(CODE_BLOCK[i+1]);
                if(nxt<=x)
                    throw indentation_error(R"(expected an indented block after 'if' statement)");
            }
            const bool check=cast_to_bool(interpreter_arithmetic(s.substr(x+3,pos-x-3)));
            if(!check)
                running_code.back().pre_state=2,running_code.back().pre_pos=i;
            else
                running_code.back().pre_state=1,running_code.back().pre_pos=i;
            running_code.emplace_back(nxt,0,i);
            if(check)
                now=nxt;
        }
        else if(s.size()>=x+4&&s.substr(x,4)=="elif"&&(s.size()==x+4||s[x+4]==' '||s[x+4]=='\t'))
        {
            int pos=search_first_target(s,':');
            while(pos==-1)
            {
                if(i+1==CODE_BLOCK.size())
                    throw invalid_expression(R"(expected ':')");
                s+=" "+CODE_BLOCK[++i];
                pos=search_first_target(s,':');
            }
            auto following=remove_blank(s.substr(pos+1));
            int nxt;
            if(!following.empty())
            {
                for(int ii=1;ii+pos<s.size();ii++)
                    CODE_BLOCK[i].pop_back();
                if(i+1!=CODE_BLOCK.size()&&get_indentation(CODE_BLOCK[i+1])>x)
                    throw indentation_error("unexpected indent");
                std::string a;
                for(int i_=0;i_<=x;i_++)
                    a+=' ';
                nxt=x+1;
                CODE_BLOCK.insert(CODE_BLOCK.begin()+i+1,a+following);
            }
            else
            {
                if(i+1==CODE_BLOCK.size())
                    throw indentation_error(R"(expected an indented block after 'elif' statement)");
                nxt=get_indentation(CODE_BLOCK[i+1]);
                if(nxt<=x)
                    throw indentation_error(R"(expected an indented block after 'elif' statement)");
            }
            if(running_code.back().pre_state!=1&&running_code.back().pre_state!=2)
                throw indentation_error(R"(expected 'if' statement before a 'elif' statement)");
            if(running_code.back().pre_state==1)
                running_code.back().pre_state=3,running_code.back().pre_pos=i,running_code.emplace_back(nxt,0,i);
            else
            {
                const bool check=cast_to_bool(interpreter_arithmetic(s.substr(x+5,pos-x-5)));
                if(!check)
                    running_code.back().pre_state=4,running_code.back().pre_pos=i;
                else
                    running_code.back().pre_state=3,running_code.back().pre_pos=i;
                running_code.emplace_back(nxt,0,i);
                if(check)
                    now=nxt;
            }
        }
        else if(s.size()>=x+4&&s.substr(x,4)=="else")
        {
            int pos=search_first_target(s,':');
            if(pos==-1)
                throw invalid_expression(R"(expected ':')");
            auto following=remove_blank(s.substr(pos+1));
            int nxt;
            if(!following.empty())
            {
                for(int ii=1;ii+pos<s.size();ii++)
                    CODE_BLOCK[i].pop_back();
                if(i+1!=CODE_BLOCK.size()&&get_indentation(CODE_BLOCK[i+1])>x)
                    throw indentation_error("unexpected indent");
                std::string a;
                for(int i_=0;i_<=x;i_++)
                    a+=' ';
                nxt=x+1;
                CODE_BLOCK.insert(CODE_BLOCK.begin()+i+1,a+following);
            }
            else
            {
                if(i+1==CODE_BLOCK.size())
                    throw indentation_error(R"(expected an indented block after 'else' statement)");
                nxt=get_indentation(CODE_BLOCK[i+1]);
                if(nxt<=x)
                    throw indentation_error(R"(expected an indented block after 'else' statement)");
            }
            if(running_code.back().pre_state!=1&&running_code.back().pre_state!=2&&running_code.back().pre_state!=3&&running_code.back().pre_state!=4)
                throw indentation_error(R"(expected 'if' or 'elif' statement before a 'else' statement)");
            if(running_code.back().pre_state==1||running_code.back().pre_state==3)
                running_code.back().pre_state=0,running_code.back().pre_pos=i,running_code.emplace_back(nxt,0,i);
            else
                running_code.back().pre_state=0,running_code.back().pre_pos=i,running_code.emplace_back(nxt,0,i),now=nxt;
        }
        else if(s.size()>=x+5&&s.substr(x,5)=="while"&&(s.size()==x+5||s[x+5]==' '||s[x+5]=='\t'))
        {
            int pos=search_first_target(s,':');
            while(pos==-1)
            {
                if(i+1==CODE_BLOCK.size())
                    throw invalid_expression(R"(expected ':')");
                s+=" "+CODE_BLOCK[++i];
                pos=search_first_target(s,':');
            }
            auto following=remove_blank(s.substr(pos+1));
            int nxt;
            if(!following.empty())
            {
                for(int ii=1;ii+pos<s.size();ii++)
                    CODE_BLOCK[i].pop_back();
                if(i+1!=CODE_BLOCK.size()&&get_indentation(CODE_BLOCK[i+1])>x)
                    throw indentation_error("unexpected indent");
                std::string a;
                for(int i_=0;i_<=x;i_++)
                    a+=' ';
                nxt=x+1;
                CODE_BLOCK.insert(CODE_BLOCK.begin()+i+1,a+following);
            }
            else
            {
                if(i+1==CODE_BLOCK.size())
                    throw indentation_error(R"(expected an indented block after 'while' statement)");
                nxt=get_indentation(CODE_BLOCK[i+1]);
                if(nxt<=x)
                    throw indentation_error(R"(expected an indented block after 'while' statement)");
            }
            const bool check=cast_to_bool(interpreter_arithmetic(s.substr(x+6,pos-x-6)));
            if(!check)
                running_code.back().pre_state=0,running_code.back().pre_pos=i;
            else
                running_code.back().pre_state=5,running_code.back().pre_pos=i;
            running_code.emplace_back(nxt,0,i);
            if(check)
                now=nxt;
        }
        else if(s.size()>=x+3&&s.substr(x,3)=="def"&&(s.size()==x+3||s[x+3]==' '||s[x+3]=='\t'))
        {
            int pos=search_first_target(s,':');
            while(pos==-1)
            {
                if(i+1==CODE_BLOCK.size())
                    throw invalid_expression(R"(expected ':')");
                s+=" "+CODE_BLOCK[++i];
                pos=search_first_target(s,':');
            }
            auto following=remove_blank(s.substr(pos+1));
            int nxt;
            if(!following.empty())
            {
                for(int ii=1;ii+pos<s.size();ii++)
                    CODE_BLOCK[i].pop_back();
                if(i+1!=CODE_BLOCK.size()&&get_indentation(CODE_BLOCK[i+1])>x)
                    throw indentation_error("unexpected indent");
                std::string a;
                for(int i_=0;i_<=x;i_++)
                    a+=' ';
                nxt=x+1;
                CODE_BLOCK.insert(CODE_BLOCK.begin()+i+1,a+following);
            }
            else
            {
                if(i+1==CODE_BLOCK.size())
                    throw indentation_error(R"(expected an indented block after function definition)");
                nxt=get_indentation(CODE_BLOCK[i+1]);
                if(nxt<=x)
                    throw indentation_error(R"(expected an indented block after function definition)");
            }
            auto tmp=s.substr(x+3,pos-x-3);
            tmp=remove_blank(tmp);
            int POS=static_cast<int>(tmp.size());
            for(int j=0;j<static_cast<int>(tmp.size());j++)
                if(!is_legal(tmp[j]))
                {
                    POS=j;
                    break;
                }
            name=tmp.substr(0,POS);
            if(!is_variable(name))
                throw invalid_expression("invalid syntax");
            tmp=remove_blank(tmp.substr(POS));
            if(tmp.empty()||tmp[0]!='(')
                throw invalid_expression(R"(expected '(')");
            if(tmp.back()!=')')
                throw invalid_expression("invalid syntax");
            now_func=python_function(tmp.substr(1,tmp.size()-2));
            running_code.back()=running_information(now,0,i);
            running_code.emplace_back(nxt,6,-1);
        }
        else if(s.size()>=x+6&&s.substr(x,6)=="return"&&(s.size()==x+6||s[x+6]==' '||s[x+6]=='\t'))
        {
            if(!state)
                throw invalid_expression("('return' outside function)");
            if(s.size()<=x+7)
                return 0ll;
            std::any tmp_res=interpreter_arithmetic(s.substr(x+6),true);
            while(CNT>0)
            {
                if(i+1==CODE_BLOCK.size())
                    interpreter_arithmetic("");
                tmp_res=interpreter_arithmetic(CODE_BLOCK[++i],true);
            }
            return interpreter_value(tmp_res);
        }
        else
        {
            if(const std::string tmp_str=remove_back_blank(s.substr(x)); tmp_str=="pass"||tmp_str.empty()){}
            else if(tmp_str=="continue")
            {
                while(!running_code.empty()&&running_code.back().pre_state!=5)
                    running_code.pop_back();
                if(running_code.empty())
                    throw invalid_expression(R"('continue' not properly in loop)");
                i=running_code.back().pre_pos-1;
                now=running_code.back().indentation_count;
            }
            else if(tmp_str=="break")
            {
                while(!running_code.empty()&&running_code.back().pre_state!=5)
                    running_code.pop_back();
                if(running_code.empty())
                    throw invalid_expression(R"('break' outside loop)");
                now=running_code.back().indentation_count;
                running_code.back().pre_state=0;
            }
            else
            {
                std::any tmp_res=interpreter_arithmetic(s.substr(x),true);
                while(CNT>0)
                {
                    if(i+1==CODE_BLOCK.size())
                        interpreter_arithmetic("");
                    tmp_res=interpreter_arithmetic(CODE_BLOCK[++i],true);
                }
                if(tmp_res.type()!=typeid(long long)&&tmp_res.type()!=typeid(wrap)&&!state)
                    print_to_screen(tmp_res),std::cout<<std::endl;
                running_code.back().pre_pos=i,running_code.back().pre_state=0;
            }
        }
    }
    return 0ll;
}

inline std::vector<std::string> CODE_BLOCK;
inline void interpreter(const std::string &a)
{
    static int INIT=false;
    if(!INIT)
    {
        INIT=true;
        python_function tmp("a");
        tmp.id=-1;
        variable["int"]=tmp;
        tmp.id=-2;
        variable["float"]=tmp;
        tmp.id=-3;
        variable["bool"]=tmp;
        tmp.id=-4;
        variable["str"]=tmp;
        tmp.id=-5;
        variable["tuple"]=tmp;
        tmp.id=-6;
        variable["print"]=tmp;
    }
    static int INTERPRETER_STATE_=false;
    if(!INTERPRETER_STATE_&&a=="begin")
        INTERPRETER_STATE_=true;
    else if(INTERPRETER_STATE_&&a=="end")
        INTERPRETER_STATE_=false;
    else
    {
        if(!CODE_BLOCK.empty()&&!CODE_BLOCK.back().empty()&&CODE_BLOCK.back().back()=='\\')
            CODE_BLOCK.back().pop_back(),CODE_BLOCK.back()+=" "+remove_back_blank(a);
        else
            CODE_BLOCK.push_back(remove_back_blank(a));
    }
    if(!INTERPRETER_STATE_)
    {
        raw_expression_.clear();CNT=0;
        const auto tmp_CODE_BLOCK=CODE_BLOCK;
        CODE_BLOCK.clear();
        interpreter_block(tmp_CODE_BLOCK,0,&variable);
    }
}

#endif