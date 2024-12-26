#ifndef PARSER_H
#define PARSER_H

#include<any>
#include<unordered_map>
#include<memory>
#include<iostream>
#include"intelligent.h"
#include"int2048.h"
#include"float2048.h"

/*
 *code-translator-processor-builder-> expression tree(executable structure)
 *a series of expression tree-code_block executor-single executor-partial executor->result
 */

using OBJECT=std::any;
using POINTER=std::shared_ptr<OBJECT>;
using BIND=std::shared_ptr<POINTER>;
using BIND_PTR=std::shared_ptr<BIND>;
using DICT=std::unordered_map<std::string,BIND>;
using DICT_PTR=std::shared_ptr<DICT>;

/*
 *This part of code is for some universal tools
 */

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
inline bool is_legal_(const char s){return is_letter(s)||s=='_';}
inline bool is_legal(const char s){return is_legal_(s)||is_number(s);}
inline int get_indentation(const std::string &a)
{
    for(int i=0;i<static_cast<int>(a.size());i++)
        if(a[i]=='\t')
            throw indentation_error("prohibited use of tabs as indentation");
        else if(a[i]!=' ')
            return i;
    return 0;
}

const std::vector<std::string> NO_VAR=
{
    "and","or","not","True","False","None","is","in",
    "def","return","break","continue","if","elif","else","while","global","nonlocal"
};
inline bool is_variable(const std::string &s)
{
    if(s.empty()||!is_legal_(s[0]))
        return false;
    for(const auto t:s)
        if(!is_legal(t))
            return false;
    for(const auto& t:NO_VAR)
        if(s==t)
            return false;
    return true;
}

/*
 *This part of code is for types
 */
enum class OPERATOR;
enum class TYPE{INT,FLOAT,BOOL,STR,TUPLE,NONE,FUNCTION};
constexpr std::string TYPENAME[]={"int","float","bool","str","tuple","none","function"};
class Tuple
{
    friend Tuple operator+(const Tuple &a,const Tuple &b)
    {
        Tuple c=a;
        for(const auto& t:b.val)
            c.val.push_back(t);
        return c;
    }
    friend Tuple operator*(const Tuple &a,const int65536 &b)
    {
        Tuple c;
        for(int65536 i(1);i<=b;++i)
            c=c+a;
        return c;
    }
    friend Tuple operator*(const int65536 &a,const Tuple &b){return b*a;}
    friend std::strong_ordering operator<=>(const Tuple&,const Tuple&);
    friend bool operator==(const Tuple&,const Tuple&);
public:
    std::vector<std::any> val;
    Tuple()=default;
    explicit Tuple(const std::any &a){val.push_back(a);}
    [[nodiscard]] int SIZE()const{return static_cast<int>(val.size());}
    std::any operator[](const int &a)const
    {
        if(a<-static_cast<int>(val.size())||a>=static_cast<int>(val.size()))
            throw undefined_behavior("tuple index out of range");
        if(a<0)
            return val[a+static_cast<int>(val.size())];
        return val[a];
    }
    std::any operator[](const int65536 &a)const{return val[static_cast<int>(a)];}
    explicit operator bool()const{return !val.empty();}
};
class str
{
    friend str operator+(const str &a,const str &b)
    {
        str c=a;
        for(const auto t:b.val)
            c.val.push_back(t);
        return c;
    }
    friend str operator*(const str &a,const int65536 &b)
    {
        str c;
        const int tmp=static_cast<int>(b);
        for(int i=1;i<=tmp;++i)
            c=c+a;
        return c;
    }
    friend str operator*(const int65536 &a,const str &b){return b*a;}
    friend std::strong_ordering operator<=>(const str &a,const str &b)
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
    friend bool operator==(const str &a,const str &b)
    {
        if(a.SIZE()!=b.SIZE())
            return false;
        for(int i=0;i<a.SIZE();i++)
            if(a[i].val!=b[i].val)
                return false;
        return true;
    }
private:
    std::string val;
public:
    str()=default;
    explicit str(const char a){val.push_back(a);}
    explicit str(std::string a):val(std::move(a)){}
    [[nodiscard]] int SIZE()const{return static_cast<int>(val.size());}
    str operator[](const int &a)const
    {
        if(a<-static_cast<int>(val.size())||a>=static_cast<int>(val.size()))
            throw undefined_behavior("str index out of range");
        if(a<0)
            return str(val[a+static_cast<int>(val.size())]);
        return str(val[a]);
    }
    str operator[](const int65536 &a)const{return (*this)[static_cast<int>(a)];}
    str operator+=(const str &a){return *this=*this+a;}
    str operator*=(const int65536 &a){return *this=*this*a;}
    explicit operator bool()const{return !val.empty();}
    explicit operator std::string()const{return val;}
};
inline int COUNT_OF_FUNCTION=0;
class python_function
{
    friend bool operator==(const python_function &a,const python_function &b){return a.id==b.id;}
public:
    std::vector<std::pair<int,std::vector<std::any>>> code;//the code block and indentation of itself
    int id=0;//the id of itself
    std::vector<std::string> parameter;//the parameter table of itself
    std::unordered_map<std::string,OBJECT> para_dict;//the parameter dict of itself
    DICT_PTR NONLOCAL_DICT=nullptr,LOCAL_DICT=nullptr;//the nonlocal and local dict of itself
    std::vector<DICT_PTR> PRE_DICT;
    python_function()=default;
    explicit python_function(int);//to build its parameter table and dict
    void call(int);//to build its local dict(parameter dict)
};
enum class RUNNING_STATE
{
    NORMAL,SUCCESSFUL_IF,FAILED_IF,SUCCESSFUL_ELIF,FAILED_ELIF,
    SUCCESSFUL_WHILE,FAILED_WHILE,BROKEN_WHILE,FUNCTION_CONTENT
};
struct running_information
{
    int indentation_count,pre_pos;
    RUNNING_STATE pre_state;
    running_information(const int &a,const int &b,const RUNNING_STATE &c):indentation_count(a),pre_pos(b),pre_state(c){}
};
inline std::string type_name(const std::any &x)
{
    if(x.type()==typeid(int65536))
        return "int";
    if(x.type()==typeid(float2048<>))
        return "float";
    if(x.type()==typeid(bool))
        return "bool";
    if(x.type()==typeid(str))
        return "str";
    if(x.type()==typeid(Tuple))
        return "tuple";
    if(x.type()==typeid(python_function))
        return "function";
    if(x.type()==typeid(long long))
        return "none";
    if(x.type()==typeid(OPERATOR))
        return "operator";
    if(x.type()==typeid(std::string))
        return "variable";
    if(x.type()==typeid(POINTER))
        return "pointer";
    if(x.type()==typeid(BIND))
        return "bind";
    if(x.type()==typeid(BIND_PTR))
        return "bind_ptr";
    if(x.type()==typeid(DICT))
        return "dict";
    if(x.type()==typeid(DICT_PTR))
        return "dict_ptr";
    return "unknown";
}

inline float2048<> cast_to_float(const OBJECT &a)
{
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
inline int65536 cast_to_int(const OBJECT &a)
{
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
inline bool cast_to_bool(const OBJECT &a)
{
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
inline str cast_to_str(const OBJECT &a)
{
    if(a.type()==typeid(int65536))
        return str(std::string(std::any_cast<int65536>(a)));
    if(a.type()==typeid(float2048<>))
    {
        const std::stringstream ss;
        std::streambuf* buffer=std::cout.rdbuf();
        std::cout.rdbuf(ss.rdbuf());
        std::cout<<std::fixed<<std::setprecision(12)<<std::any_cast<float2048<>>(a);
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
inline Tuple cast_to_tuple(const OBJECT &a)
{
    if(a.type()==typeid(str))
    {
        Tuple tmp;
        const auto S=std::any_cast<str>(a);
        for(int i=0;i<S.SIZE();i++)
            tmp.val.emplace_back(S[i]);
        return tmp;
    }
    if(a.type()==typeid(Tuple))
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

/*
 *This part of code is for objects and variables
 */
inline DICT_PTR GLOBAL_DICT=std::make_shared<DICT>(DICT{}),NONLOCAL_DICT,LOCAL_DICT;
inline std::unordered_map<std::string,DICT_PTR> MY_DICT;
inline std::unordered_map<std::string,int> BIND_DICT;

const std::vector<std::string> MY_KEY=
{
    "def","return","break","continue","pass","if","elif","else","while","global","nonlocal"
};
enum class KEYWORD
{
    DEF,RETURN,BREAK,CONTINUE,PASS,IF,ELIF,ELSE,WHILE,GLOBAL,NONLOCAL,
    ILLEGAL
};

//It is a function to get the value of variable(get PTR) or literal(get OBJ).
inline std::any get_value(const std::any &a)
{
    if(a.type()!=typeid(std::string))
        return a;
    const auto s=std::any_cast<std::string>(a);
    if(BIND_DICT.contains(s))
    {
        const auto x=BIND_DICT[s];
        if(!x)
            MY_DICT[s]=LOCAL_DICT;
        else if(x==1)
            MY_DICT[s]=NONLOCAL_DICT;
        else
            MY_DICT[s]=GLOBAL_DICT;
    }
    if(MY_DICT.contains(s))
    {
        const auto tmp=MY_DICT[s];
        if((*tmp)[s]==nullptr)
            throw undefined_behavior("name \'"+s+"\' is not defined");
        return *(*tmp)[s];
    }
    if(NONLOCAL_DICT!=nullptr&&NONLOCAL_DICT->contains(s)&&*(*NONLOCAL_DICT)[s]!=nullptr)
        return *(*NONLOCAL_DICT)[s];
    if(!GLOBAL_DICT->contains(s))
        throw undefined_behavior("name \'"+s+"\' is not defined");
    const BIND tmp=(*GLOBAL_DICT)[s];
    if(*tmp==nullptr)
        throw undefined_behavior("name \'"+s+"\' is not defined");
    return *tmp;
}
inline OBJECT remove_ptr(const std::any &a)
{
    if(a.type()==typeid(POINTER))
        return *std::any_cast<POINTER>(a);
    return a;
}

/*
 *This part of code is for operators
 */
enum class OPERATOR
{
    ADD,SUB,MUL,DIV,MOD,IDIV,POW,
    LPAR,RPAR,LBRA,RBRA,
    LE,LEQ,GE,GEQ,EQ,NEQ,IS,IN,IS_NOT,NOT_IN,
    OR,AND,NOT,
    COMMA,DOT,COLON,
    ASSIGN,ADDASS,SUBASS,MULASS,DIVASS,MODASS,IDIVASS,POWASS,REAASS,
    PLUS,MINUS,CALL,CALL_,ALLO,
    ILLEGAL
};
inline int priority(const OPERATOR &x)
{
    switch(x)
    {
        case OPERATOR::LPAR:
        case OPERATOR::LBRA:
        case OPERATOR::CALL:return 0;
        case OPERATOR::ASSIGN:
        case OPERATOR::ADDASS:
        case OPERATOR::SUBASS:
        case OPERATOR::MULASS:
        case OPERATOR::DIVASS:
        case OPERATOR::MODASS:
        case OPERATOR::IDIVASS:
        case OPERATOR::POWASS:
        case OPERATOR::REAASS:return 1;
        case OPERATOR::COMMA:return 2;
        case OPERATOR::ALLO:return 3;
        case OPERATOR::OR:return 4;
        case OPERATOR::AND:return 5;
        case OPERATOR::NOT:return 6;
        case OPERATOR::LE:
        case OPERATOR::LEQ:
        case OPERATOR::GE:
        case OPERATOR::GEQ:
        case OPERATOR::EQ:
        case OPERATOR::NEQ:
        case OPERATOR::IS:
        case OPERATOR::IN:
        case OPERATOR::IS_NOT:
        case OPERATOR::NOT_IN:return 7;
        case OPERATOR::ADD:
        case OPERATOR::SUB:return 8;
        case OPERATOR::MUL:
        case OPERATOR::DIV:
        case OPERATOR::IDIV:
        case OPERATOR::MOD:return 9;
        case OPERATOR::PLUS:
        case OPERATOR::MINUS:return 10;
        case OPERATOR::POW:return 11;
        case OPERATOR::RBRA:
        case OPERATOR::CALL_:
        case OPERATOR::DOT:return 12;
        case OPERATOR::RPAR:return 13;
        default:return -1;
    }
}
namespace operator_
{
    inline OBJECT operator_add(const OBJECT &a,const OBJECT &b)
    {
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
    inline OBJECT operator_plus(const OBJECT &a)
    {
        if(a.type()==typeid(str)||a.type()==typeid(Tuple)||a.type()==typeid(python_function)||a.type()==typeid(long long))
            throw undefined_behavior(R"(bad operand type for unary +: ')"+type_name(a)+R"(')");
        if(a.type()==typeid(float2048<>))
            return cast_to_float(a);
        return cast_to_int(a);
    }
    inline OBJECT operator_sub(const OBJECT &a,const OBJECT &b)
    {
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for -: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for -: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)-cast_to_float(b);
        return cast_to_int(a)-cast_to_int(b);
    }
    inline OBJECT operator_minus(const OBJECT &a)
    {
        if(a.type()==typeid(str)||a.type()==typeid(Tuple)||a.type()==typeid(python_function)||a.type()==typeid(long long))
            throw undefined_behavior(R"(bad operand type for unary -: ')"+type_name(a)+R"(')");
        if(a.type()==typeid(float2048<>))
            return -cast_to_float(a);
        return -cast_to_int(a);
    }
    inline OBJECT operator_mul(const OBJECT &a,const OBJECT &b)
    {
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
    inline OBJECT operator_div(const OBJECT &a,const OBJECT &b)
    {
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for /: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for /: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        return cast_to_float(a)/cast_to_float(b);
    }
    inline OBJECT operator_idiv(const OBJECT &a,const OBJECT &b)
    {
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for //: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for //: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return floor(cast_to_float(operator_div(a,b)));
        return cast_to_int(a)/cast_to_int(b);
    }
    inline OBJECT operator_mod(const OBJECT &a,const OBJECT &b)
    {
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for %: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for %: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>))
            return cast_to_float(a)%cast_to_float(b);
        return cast_to_int(a)%cast_to_int(b);
    }
    inline OBJECT operator_pow(const OBJECT &a,const OBJECT &b)
    {
        if(a.type()==typeid(python_function)||b.type()==typeid(python_function)||a.type()==typeid(long long)||b.type()==typeid(long long))
            throw undefined_behavior(R"(unsupported operand type(s) for ** or pow(): ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(str)||b.type()==typeid(str)||a.type()==typeid(Tuple)||b.type()==typeid(Tuple))
            throw undefined_behavior(R"(unsupported operand type(s) for ** or pow(): ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        if(a.type()==typeid(float2048<>)||b.type()==typeid(float2048<>)||cast_to_float(b)<float2048(0))
            return cast_to_float(a)^cast_to_float(b);
        return cast_to_int(a)^cast_to_int(b);
    }
    inline OBJECT operator_less(const OBJECT &a,const OBJECT &b)
    {
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
    inline OBJECT operator_less_eq(const OBJECT &a,const OBJECT &b)
    {
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
    inline OBJECT operator_greater(const OBJECT &a,const OBJECT &b)
    {
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
    inline OBJECT operator_greater_eq(const OBJECT &a,const OBJECT &b)
    {
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
    inline OBJECT operator_equal(const OBJECT &a,const OBJECT &b)
    {
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
    inline OBJECT operator_not_equal(const OBJECT &a,const OBJECT &b)
    {
        return !std::any_cast<bool>(operator_equal(a,b));
    }
    inline OBJECT operator_or(const OBJECT &a,const OBJECT &b)
    {
        if(cast_to_bool(a))
            return a;
        return b;
    }
    inline OBJECT operator_and(const OBJECT &a,const OBJECT &b)
    {
        if(!cast_to_bool(a))
            return a;
        return b;
    }
    inline OBJECT operator_not(const OBJECT &a)
    {
        return !cast_to_bool(a);
    }
    inline std::any operator_index(const OBJECT &a,const OBJECT &b)
    {
        if(b.type()!=typeid(int65536)&&b.type()!=typeid(bool))
            throw undefined_behavior(R"(unsupported operand type(s) for []: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
        const int tmp=static_cast<int>(cast_to_int(b));
        if(a.type()==typeid(str))
            return std::any_cast<str>(a)[tmp];
        if(a.type()==typeid(Tuple))
            return std::any_cast<Tuple>(a)[tmp];
        throw undefined_behavior(R"(unsupported operand type(s) for []: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    }
}

inline std::strong_ordering operator<=>(const Tuple &a, const Tuple &b)
{
    const int siz=std::min(a.SIZE(),b.SIZE());
    for(int i=0;i<siz;i++)
    {
        if(std::any_cast<bool>(operator_::operator_less(a[i],b[i])))
            return std::strong_ordering::less;
        if(std::any_cast<bool>(operator_::operator_greater(a[i],b[i])))
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
        if(std::any_cast<bool>(operator_::operator_not_equal(a[i],b[i])))
            return false;
    return true;
}

/*
 *This part of code is for translator
 */
//some tools for translator
inline std::string operator_what(const OPERATOR &a)
{
    switch(a)
    {
        case OPERATOR::ADD:return "ADD";
        case OPERATOR::SUB:return "SUB";
        case OPERATOR::MUL:return "MUL";
        case OPERATOR::DIV:return "DIV";
        case OPERATOR::MOD:return "MOD";
        case OPERATOR::IDIV:return "IDIV";
        case OPERATOR::POW:return "POW";
        case OPERATOR::LPAR:return "LPAR";
        case OPERATOR::RPAR:return "RPAR";
        case OPERATOR::LBRA:return "LBRA";
        case OPERATOR::RBRA:return "RBRA";
        case OPERATOR::LE:return "LE";
        case OPERATOR::LEQ:return "LEQ";
        case OPERATOR::GE:return "GE";
        case OPERATOR::GEQ:return "GEQ";
        case OPERATOR::EQ:return "EQ";
        case OPERATOR::NEQ:return "NEQ";
        case OPERATOR::IS:return "IS";
        case OPERATOR::IN:return "IN";
        case OPERATOR::IS_NOT:return "IS_NOT";
        case OPERATOR::NOT_IN:return "NOT_IN";
        case OPERATOR::OR:return "OR";
        case OPERATOR::AND:return "AND";
        case OPERATOR::NOT:return "NOT";
        case OPERATOR::COMMA:return "COMMA";
        case OPERATOR::DOT:return "DOT";
        case OPERATOR::COLON:return "COLON";
        case OPERATOR::ASSIGN:return "ASSIGN";
        case OPERATOR::ADDASS:return "ADDASS";
        case OPERATOR::SUBASS:return "SUBASS";
        case OPERATOR::MULASS:return "MULASS";
        case OPERATOR::DIVASS:return "DIVASS";
        case OPERATOR::MODASS:return "MODASS";
        case OPERATOR::IDIVASS:return "IDIVASS";
        case OPERATOR::POWASS:return "POWASS";
        case OPERATOR::REAASS:return "REAASS";
        case OPERATOR::PLUS:return "PLUS";
        case OPERATOR::MINUS:return "MINUS";
        case OPERATOR::CALL:return "CALL";
        case OPERATOR::CALL_:return "CALL_";
        case OPERATOR::ALLO:return "ALLO";
        case OPERATOR::ILLEGAL:return "ILLEGAL";
        default:return "UNKNOWN";
    }
}
inline std::string keyword_what(const KEYWORD &a)
{
    switch(a)
    {
        case KEYWORD::DEF:return "DEF";
        case KEYWORD::RETURN:return "RETURN";
        case KEYWORD::BREAK:return "BREAK";
        case KEYWORD::CONTINUE:return "CONTINUE";
        case KEYWORD::PASS:return "PASS";
        case KEYWORD::IF:return "IF";
        case KEYWORD::ELIF:return "ELIF";
        case KEYWORD::ELSE:return "ELSE";
        case KEYWORD::WHILE:return "WHILE";
        case KEYWORD::GLOBAL:return "GLOBAL";
        case KEYWORD::NONLOCAL:return "NONLOCAL";
        default:return "ILLEGAL";
    }
}

inline void print_to_screen(const OBJECT &a,const bool op=false)
{
    if(a.type()==typeid(int65536))
        std::cout<<std::any_cast<int65536>(a);
    if(a.type()==typeid(float2048<>))
        std::cout<<std::setprecision(12)<<std::any_cast<float2048<>>(a);
    if(a.type()==typeid(bool))
        std::cout<<(std::any_cast<bool>(a)?"True":"False");
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
                    else if(tmp[i]=='\'')
                        std::cout<<'\'';
                    else if(tmp[i]=='\"')
                        std::cout<<'\"';
                }
                else
                    std::cout<<tmp[i];
    }
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
    if(a.type()==typeid(OPERATOR))
        std::cout<<"OPERATOR-"<<operator_what(std::any_cast<OPERATOR>(a));
    if(a.type()==typeid(python_function))
        std::cout<<"function"<<std::any_cast<python_function>(a).id;
    if(a.type()==typeid(long long))
        std::cout<<"None";
    if(a.type()==typeid(std::string))
        std::cout<<"variable-"<<std::any_cast<std::string>(a);
    if(a.type()==typeid(KEYWORD))
        std::cout<<"KEYWORD-"<<keyword_what(std::any_cast<KEYWORD>(a));
}
inline bool is_operator(const char s)
{
    return s=='+'||s=='-'||s=='*'||s=='/'||s=='%'||
        s=='<'||s=='>'||s=='='||s=='!'||s=='('||s==')'||s=='['||s==']'||
            s==','||s=='.'||s==':'||s=='}';
}
inline OPERATOR translator_operator(const std::string &s)
{
    if(s=="+")
        return OPERATOR::ADD;
    if(s=="-")
        return OPERATOR::SUB;
    if(s=="*")
        return OPERATOR::MUL;
    if(s=="/")
        return OPERATOR::DIV;
    if(s=="%")
        return OPERATOR::MOD;
    if(s=="//")
        return OPERATOR::IDIV;
    if(s=="**")
        return OPERATOR::POW;
    if(s=="(")
        return OPERATOR::LPAR;
    if(s==")")
        return OPERATOR::RPAR;
    if(s=="[")
        return OPERATOR::LBRA;
    if(s=="]")
        return OPERATOR::RBRA;
    if(s=="<")
        return OPERATOR::LE;
    if(s=="<=")
        return OPERATOR::LEQ;
    if(s==">")
        return OPERATOR::GE;
    if(s==">=")
        return OPERATOR::GEQ;
    if(s=="==")
        return OPERATOR::EQ;
    if(s=="!=")
        return OPERATOR::NEQ;
    if(s=="or")
        return OPERATOR::OR;
    if(s=="and")
        return OPERATOR::AND;
    if(s=="not")
        return OPERATOR::NOT;
    if(s==",")
        return OPERATOR::COMMA;
    if(s==".")
        return OPERATOR::DOT;
    if(s=="=")
        return OPERATOR::ASSIGN;
    if(s=="+=")
        return OPERATOR::ADDASS;
    if(s=="-=")
        return OPERATOR::SUBASS;
    if(s=="*=")
        return OPERATOR::MULASS;
    if(s=="/=")
        return OPERATOR::DIVASS;
    if(s=="%=")
        return OPERATOR::MODASS;
    if(s=="//=")
        return OPERATOR::IDIVASS;
    if(s=="**=")
        return OPERATOR::POWASS;
    if(s==":=")
        return OPERATOR::REAASS;
    if(s==":")
        return OPERATOR::COLON;
    if(s=="is")
        return OPERATOR::IS;
    if(s=="in")
        return OPERATOR::IN;
    return OPERATOR::ILLEGAL;
}
inline KEYWORD translator_keyword(const std::string &s)
{
    for(int i=0;i<static_cast<int>(MY_KEY.size());i++)
        if(s==MY_KEY[i])
            return static_cast<KEYWORD>(i);
    return KEYWORD::ILLEGAL;
}
inline std::any translator_literal(const std::string &con)
{
    if(con=="True")
        return true;
    if(con=="False")
        return false;
    if(con=="None")
        return 0ll;
    if(!con.empty()&&(con.front()=='\''||con.front()=='\"'))
        return str(con.substr(1,con.size()-2));
    int type=0;
    for(const auto t:con)
        if(t=='.'||t=='e')
            type=1;
    try
    {
        if(type==0)
            return int65536(con);
        return float2048<>(con);
    }
    catch(...)
    {
        throw invalid_expression("invalid_expression");
    }
}
inline std::any translator_any(const std::string &s)
{
    if(const auto tmp=translator_operator(s);tmp!=OPERATOR::ILLEGAL)
        return tmp;
    if(const auto tmp=translator_keyword(s);tmp!=KEYWORD::ILLEGAL)
        return tmp;
    try
    {
        return translator_literal(s);
    }
    catch(...)
    {
        if(!is_variable(s))
            throw undefined_behavior("unknown object "+s);
        return s;
    }
}

inline int mode=0,IN_EOF=0,INDENTATION_COUNT;

/*
 *translate s into machine-identifiable language (stored in raw_expression_) and return the state of translation.
 *If normally ends,return 1,otherwise return 0.
 */
std::vector<std::any> raw_expression_;
inline int translator(const std::string &s,bool option=false)
{
    static std::string las;
    static int state=-1,shift=0;
    static std::vector<std::string> pre;
    static std::vector<std::string> sta;
    IN_EOF=0;
    if(!mode)
    {
        mode=1,INDENTATION_COUNT=get_indentation(s);
        state=-1,shift=0;
        raw_expression_.clear(),pre.clear(),las.clear(),sta.clear();
    }
    try
    {
        for(int i=0;i<static_cast<int>(s.size());i++)
        {
            const char t=s[i];
            if(i==static_cast<int>(s.size())-1&&t=='\\')
            {
                if(option)
                    throw invalid_expression("invalid syntax");
                IN_EOF=1;
                if(las=="("||las=="["||las=="{")
                    sta.push_back(las);
                if(las==")"||las=="]"||las=="}")
                {
                    if(sta.empty())
                        throw invalid_expression("unmatched \'"+las+"\'");
                    sta.pop_back();
                }
                if(state==-1)
                    return 0;
                if(state==0||state==1)
                {
                    raw_expression_.emplace_back(translator_any(las));
                    las.clear();
                    state=-1;
                    return 0;
                }
                if(state>=2&&!shift)
                    return 0;
            }
            if(state==-1)//blank mode
            {
                if(is_legal(t)||t=='.')
                    state=0,las=t;
                else if(is_operator(t))
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
                    las.clear(),state=-1,raw_expression_.emplace_back(1ll);
            }
            /*
             *non-string literal mode
             *maybe operator,keyword,literal,variable
             *we have to identify its type
             */
            else if(state==0)
            {
                if(is_legal(t)||t=='.')
                    state=0,las+=t;
                else if(is_operator(t))
                    state=1,raw_expression_.push_back(translator_any(las)),las=t;
                else if(t=='\''||t=='\"')
                {
                    if(las=="f")
                    {
                        las=t;
                        if(t=='\"')
                            state=4;
                        else
                            state=5;
                    }
                    else
                    {
                        raw_expression_.push_back(translator_any(las));
                        las=t;
                        if(t=='\"')
                            state=2;
                        else
                            state=3;
                    }
                }
                else if(is_blank(t))
                    state=-1,raw_expression_.push_back(translator_any(las)),las.clear();
                else if(t=='#')
                {
                    state=-1;
                    raw_expression_.push_back(translator_any(las));
                    break;
                }
                else
                    las.clear(),state=-1,raw_expression_.emplace_back(1ll);
            }
            else if(state==1)// operator mode
            {
                if(las=="("||las=="["||las=="{")
                    sta.push_back(las);
                if(las==")"||las=="]"||las=="}")
                {
                    if(sta.empty())
                        throw invalid_expression("unmatched \'"+las+"\'");
                    sta.pop_back();
                }
                if(las=="}")
                {
                    if(pre.empty())
                        throw invalid_expression("unmatched \'}\'");
                    raw_expression_.emplace_back(OPERATOR::RPAR);
                    raw_expression_.emplace_back(OPERATOR::RPAR);
                    raw_expression_.emplace_back(OPERATOR::ADD);
                    state=-1;
                    i--;
                    if(pre.back()=="\"")
                        state=4,las="\"";
                    else
                        state=5,las="\'";
                    pre.pop_back();
                    continue;
                }
                if(is_legal(t)||t=='.')
                    state=0,raw_expression_.emplace_back(translator_operator(las)),las=t;
                else if(is_operator(t))
                {
                    if((t=='='&&(las=="="||las=="<"||las==">"||las=="!"||las=="+"||las=="-"||las=="*"||las=="/"||las=="//"||las=="%"||las=="**"||las==":"))
                        ||(las=="*"&&t=='*')||(las=="/"&&t=='/'))
                        state=1,las+=t;
                    else
                        state=1,raw_expression_.emplace_back(translator_operator(las)),las=t;
                }
                else if(t=='\"')
                    state=2,raw_expression_.emplace_back(translator_operator(las)),las=t;
                else if(t=='\'')
                    state=3,raw_expression_.emplace_back(translator_operator(las)),las=t;
                else if(is_blank(t))
                    state=-1,raw_expression_.emplace_back(translator_operator(las)),las.clear();
                else if(t=='#')
                {
                    state=-1;
                    raw_expression_.emplace_back(translator_operator(las));
                    break;
                }
                else
                    las.clear(),state=-1,raw_expression_.emplace_back(1ll);
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
                        raw_expression_.emplace_back(translator_literal(las)),las.clear(),state=-1;
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
                        raw_expression_.emplace_back(translator_literal(las)),las.clear(),state=-1;
                }
            }
            else if(state==4)// f"-string mode
            {
                if(t=='{')
                {
                    if(i+1!=s.size()&&s[i+1]=='{')
                        i++,las+='{';
                    else
                    {
                        las+="\"",pre.emplace_back("\"");
                        raw_expression_.push_back(translator_any(las));
                        las.clear();
                        raw_expression_.emplace_back(OPERATOR::ADD);
                        python_function tmp;tmp.id=-4;
                        raw_expression_.emplace_back(tmp);
                        raw_expression_.emplace_back(OPERATOR::LPAR);
                        raw_expression_.emplace_back(OPERATOR::LPAR);
                        state=-1,sta.push_back("{");
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
                        raw_expression_.emplace_back(translator_literal(las)),las.clear(),state=-1;
                }
            }
            else if(state==5)// f'-string mode
            {
                if(t=='{')
                {
                    if(i+1!=s.size()&&s[i+1]=='{')
                        i++,las+='{';
                    else
                    {
                        las+="\'",pre.emplace_back("\'");
                        raw_expression_.push_back(translator_any(las));
                        las.clear();
                        raw_expression_.emplace_back(OPERATOR::ADD);
                        python_function tmp;tmp.id=-4;
                        raw_expression_.emplace_back(tmp);
                        raw_expression_.emplace_back(OPERATOR::LPAR);
                        raw_expression_.emplace_back(OPERATOR::LPAR);
                        state=-1,sta.emplace_back("{");
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
                        raw_expression_.emplace_back(translator_literal(las)),las.clear(),state=-1;
                }
            }
        }
        if(las=="("||las=="["||las=="{")
            sta.push_back(las);
        if(las==")"||las=="]"||las=="}")
        {
            if(sta.empty())
                throw invalid_expression("unmatched \'"+las+"\'");
            sta.pop_back();
        }
        if(las=="}")
        {
            if(pre.empty())
                throw invalid_expression("unmatched \'}\'");
            raw_expression_.emplace_back(OPERATOR::RPAR);
            raw_expression_.emplace_back(OPERATOR::RPAR);
            raw_expression_.emplace_back(OPERATOR::ADD);
            state=-1;
            if(pre.back()=="\"")
                state=4;
            else
                state=5;
            pre.pop_back();
        }
        if(state==0||state==1)
            state=-1,raw_expression_.push_back(translator_any(las)),las.clear();
        if(!sta.empty())
        {
            if(!option)
                return 0;
            throw invalid_expression("\'"+sta.back()+R"(' was never closed)");
        }
        if(state==2||state==3)
            throw invalid_expression("unterminated string literal");
        if(state==4||state==5)
            throw invalid_expression("unterminated f-string literal");
        return mode=0,1;
    }
    catch(invalid_expression &a)
    {
        mode=0,raw_expression_.clear();
        throw invalid_expression(a.what());
    }
}

/*
 *This part of code is for processor
 *What should it do?
 *1.merge nearby str.
 *2.merge nearby operator(not in,is not)
 *3.differentiate add and plus, subtract and minus,left parenthesis and call,assign and allocator
 */
inline void processor()
{
    auto raw_expression=raw_expression_;
    raw_expression_.clear();
    for(auto t:raw_expression)
    {
        if(t.type()==typeid(long long)&&std::any_cast<long long>(t)==1ll)
            throw invalid_expression("illegal symbol");
        int ok=0;
        if(!raw_expression_.empty())
        {
            if(raw_expression_.back().type()==typeid(str)&&t.type()==typeid(str))
                raw_expression_.back()=std::any_cast<str>(raw_expression_.back())+std::any_cast<str>(t),ok=1;
            else if(raw_expression_.back().type()!=typeid(KEYWORD)&&(raw_expression_.back().type()!=typeid(OPERATOR)||
                std::any_cast<OPERATOR>(raw_expression_.back())==OPERATOR::CALL_||
                std::any_cast<OPERATOR>(raw_expression_.back())==OPERATOR::RPAR||
                std::any_cast<OPERATOR>(raw_expression_.back())==OPERATOR::RBRA)&&
                t.type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(t)==OPERATOR::LPAR)
                raw_expression_.emplace_back(OPERATOR::CALL),ok=1;
            else if(raw_expression_.back().type()==typeid(OPERATOR)&&t.type()==typeid(OPERATOR))
            {
                if(const auto now=std::any_cast<OPERATOR>(t),pre=std::any_cast<OPERATOR>(raw_expression_.back());
                    pre==OPERATOR::IS&&now==OPERATOR::NOT)
                    raw_expression_.back()=OPERATOR::IS_NOT,ok=1;
                else if(pre==OPERATOR::NOT&&now==OPERATOR::IN)
                    raw_expression_.back()=OPERATOR::NOT_IN,ok=1;
                else if(now==OPERATOR::ADD&&pre!=OPERATOR::RPAR&&pre!=OPERATOR::RBRA&&pre!=OPERATOR::CALL_)
                    raw_expression_.emplace_back(OPERATOR::PLUS),ok=1;
                else if(now==OPERATOR::SUB&&pre!=OPERATOR::RPAR&&pre!=OPERATOR::RBRA&&pre!=OPERATOR::CALL_)
                    raw_expression_.emplace_back(OPERATOR::MINUS),ok=1;
            }
        }
        else if(t.type()==typeid(OPERATOR))
        {
            const auto now=std::any_cast<OPERATOR>(t);
            if(now==OPERATOR::ADD)
                raw_expression_.emplace_back(OPERATOR::PLUS),ok=1;
            if(now==OPERATOR::SUB)
                raw_expression_.emplace_back(OPERATOR::MINUS),ok=1;
        }
        if(!ok)
            raw_expression_.push_back(t);
    }
    int C=0;
    for(auto &t:raw_expression_)
        if(t.type()==typeid(OPERATOR))
        {
            const auto tmp=std::any_cast<OPERATOR>(t);
            if(tmp==OPERATOR::CALL)
                C++;
            if(C)
            {
                if(tmp==OPERATOR::ASSIGN)
                    t=OPERATOR::ALLO;
                else if(tmp==OPERATOR::ADDASS||tmp==OPERATOR::SUBASS||tmp==OPERATOR::MULASS||tmp==OPERATOR::DIVASS||
                    tmp==OPERATOR::MODASS||tmp==OPERATOR::IDIVASS||tmp==OPERATOR::POWASS)
                    throw invalid_expression("invalid syntax");
                else if(tmp==OPERATOR::LPAR)
                    C++;
                else if(tmp==OPERATOR::RPAR)
                    C--;
            }
        }
}

/*
 *this part of code is for builder
 *about different statements?
 *-----simple statement-----
 *0.normal arithmetic statement
 *1.simple assignment statement
 *2.augmented assignment statement
 *----- block statement-----
 *3.if statement -> if as root and the left son is its condition
 *4.if with code statement -> if as root and the left son is its condition, the right son is its then-statement
 *5.elif statement -> elif as root and the left son is its condition
 *6.elif with code statement -> elif as root and the left son is its condition, the right son is its then-statement
 *7.else statement -> else as root and with no son
 *8.else with code statement -> else as root and the left son is its then-statement
 *9.while statement -> while as root and the left son is its condition
 *10.while with code statement -> while as root and the left son is its condition, the right son is its then-statement
 *11.def statement -> def as root and the left son is its parameter list
 *12.def with code statement -> def as root and the left son is its parameter list, the right son is its code block.
 *13.break statement -> break as root
 *14.continue statement -> continue as root
 *15.pass statement -> pass as root
 *16.return statement -> return as root and the left son is its value(maybe None)
 *17.global statement -> global as root and the left son is its variable to be operated
 *18.nonlocal statement -> nonlocal as root and the left son is its variable to be operated
 */
inline std::vector<int> LS,RS,FA;
inline std::vector<std::any> RES;
inline int GLOBAL_STATE=false,INTERACTIVE_MODE=false;
inline int find_colon()
{
    int CNT=0;
    for(int i=0;i<static_cast<int>(raw_expression_.size());i++)
        if(auto t=raw_expression_[i]; t.type()==typeid(OPERATOR))
            if(const auto tmp=std::any_cast<OPERATOR>(t); tmp==OPERATOR::LBRA||tmp==OPERATOR::LPAR||tmp==OPERATOR::CALL)
                CNT++;
            else if(tmp==OPERATOR::RBRA||tmp==OPERATOR::RPAR||tmp==OPERATOR::CALL_)
                CNT--;
            else if(tmp==OPERATOR::COLON&&!CNT)
                return i;
    return -1;
}
//to build an expression tree for a segment(arithmetic only)
inline int builder_arithmetic(const int l=0,const int r=static_cast<int>(raw_expression_.size()))
{
    if(l>r)
        return -1;
    for(int i=l;i<r;i++)
    {
        if(raw_expression_[i].type()==typeid(OPERATOR))
        {
            if(const auto tmp=std::any_cast<OPERATOR>(raw_expression_[i]);
                tmp==OPERATOR::ADD||tmp==OPERATOR::SUB||tmp==OPERATOR::MUL||tmp==OPERATOR::DIV||tmp==OPERATOR::MOD||
                tmp==OPERATOR::IDIV||tmp==OPERATOR::POW||tmp==OPERATOR::LE||tmp==OPERATOR::LEQ||tmp==OPERATOR::GE||
                tmp==OPERATOR::GEQ||tmp==OPERATOR::EQ||tmp==OPERATOR::NEQ||tmp==OPERATOR::IS||tmp==OPERATOR::IN||
                tmp==OPERATOR::IS_NOT||tmp==OPERATOR::NOT_IN||tmp==OPERATOR::OR||tmp==OPERATOR::AND||
                tmp==OPERATOR::ASSIGN||tmp==OPERATOR::ADDASS||tmp==OPERATOR::SUBASS||
                tmp==OPERATOR::MULASS||tmp==OPERATOR::DIVASS||tmp==OPERATOR::MODASS||tmp==OPERATOR::IDIVASS||
                tmp==OPERATOR::POWASS||tmp==OPERATOR::REAASS||tmp==OPERATOR::ALLO)
            {
                if(i+1==r||FA[i]!=-1||i==l)
                    throw invalid_expression("invalid syntax");
                int pos=i-1;
                while(FA[pos]!=-1&&priority(std::any_cast<OPERATOR>(raw_expression_[FA[pos]]))>=
                    priority(std::any_cast<OPERATOR>(raw_expression_[i])))
                    pos=FA[pos];
                if(FA[pos]!=-1)
                {
                    if(LS[FA[pos]]==pos)
                        LS[FA[pos]]=i;
                    else
                        RS[FA[pos]]=i;
                }
                FA[i]=FA[pos],FA[pos]=i,LS[i]=pos,RS[i]=i+1,FA[i+1]=i;
            }
            else if(tmp==OPERATOR::LBRA||tmp==OPERATOR::CALL)
            {
                if(i+1==r||FA[i]!=-1||i==l)
                    throw invalid_expression("invalid syntax");
                int pos=i-1;
                if(FA[pos]!=-1)
                {
                    if(LS[FA[pos]]==pos)
                        LS[FA[pos]]=i;
                    else
                        RS[FA[pos]]=i;
                }
                FA[i]=FA[pos],FA[pos]=i,LS[i]=pos,RS[i]=i+1,FA[i+1]=i;
            }
            else if(tmp==OPERATOR::NOT)
            {
                if(i+1==r||(i!=l&&FA[i]==-1))
                    throw invalid_expression("invalid syntax");
                if(i!=l)
                {
                    if(raw_expression_[i-1].type()!=typeid(OPERATOR))
                        throw invalid_expression("invalid syntax");
                    if(const auto tmp_=std::any_cast<OPERATOR>(raw_expression_[i-1]);
                        tmp_!=OPERATOR::LPAR&&tmp_!=OPERATOR::LBRA&&tmp_!=OPERATOR::OR&&tmp_!=OPERATOR::AND&&
                        tmp_!=OPERATOR::NOT&&tmp_!=OPERATOR::ASSIGN&&tmp_!=OPERATOR::ADDASS&&tmp_!=OPERATOR::SUBASS&&
                        tmp_!=OPERATOR::MULASS&&tmp_!=OPERATOR::DIVASS&&tmp_!=OPERATOR::MODASS&&
                        tmp!=OPERATOR::IDIVASS&&tmp!=OPERATOR::POWASS&&tmp!=OPERATOR::REAASS&&tmp_!=OPERATOR::CALL)
                        throw invalid_expression("invalid syntax");
                }
                FA[i+1]=i,LS[i]=i+1;
            }
            else if(tmp==OPERATOR::LPAR||tmp==OPERATOR::PLUS|tmp==OPERATOR::MINUS)
            {
                if(i+1==r)
                    throw invalid_expression(R"('(' was never closed)");
                FA[i+1]=i,LS[i]=i+1;
            }
            else if(tmp==OPERATOR::RPAR||tmp==OPERATOR::CALL_)
            {
                int pos=i-1;
                if(pos>=l&&raw_expression_[pos].type()==typeid(OPERATOR)&&
                    (std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LPAR||
                    std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::CALL||
                    std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LBRA))
                {
                    if(FA[pos]!=-1)
                    {
                        if(LS[FA[pos]]==pos)
                            LS[FA[pos]]=i;
                        else
                            RS[FA[pos]]=i;
                    }
                    FA[i]=FA[pos];
                    if(std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LBRA)
                        throw invalid_expression(R"(closing parenthesis ')' does not match opening parenthesis '[')");
                    if(std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::CALL)
                        raw_expression_[i]=OPERATOR::CALL_,LS[i]=LS[pos];
                    if(LS[i]>=0)
                        FA[LS[i]]=i;
                    if(RS[i]>=0)
                        FA[RS[i]]=i;
                    continue;
                }
                while(pos>=l&&(raw_expression_[pos].type()!=typeid(OPERATOR)||
                    std::any_cast<OPERATOR>(raw_expression_[pos])!=OPERATOR::LPAR&&
                    std::any_cast<OPERATOR>(raw_expression_[pos])!=OPERATOR::LBRA&&
                    std::any_cast<OPERATOR>(raw_expression_[pos])!=OPERATOR::CALL))
                    pos=FA[pos];
                if(pos<l)
                    throw invalid_expression(R"(unmatched ')')");
                if(std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LBRA)
                    throw invalid_expression(R"(closing parenthesis ')' does not match opening parenthesis '[')");
                if(std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::CALL)
                    raw_expression_[i]=OPERATOR::CALL_;
                FA[i]=FA[pos],LS[i]=LS[pos],RS[i]=RS[pos];
                if(LS[i]>=0)
                    FA[LS[i]]=i;
                if(RS[i]>=0)
                    FA[RS[i]]=i;
                if(FA[pos]!=-1)
                {
                    if(LS[FA[pos]]==pos)
                        LS[FA[pos]]=i;
                    else
                        RS[FA[pos]]=i;
                }
            }
            else if(tmp==OPERATOR::RBRA)
            {
                int pos=i-1;
                if(pos>=l&&raw_expression_[pos].type()==typeid(OPERATOR)&&
                    std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LBRA)
                    throw invalid_expression("invalid syntax");
                if(pos>=l&&raw_expression_[pos].type()==typeid(OPERATOR)&&
                    (std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LPAR||
                    std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::CALL))
                    throw invalid_expression(R"(closing parenthesis ']' does not match opening parenthesis '(')");
                while(pos>=l&&(raw_expression_[pos].type()!=typeid(OPERATOR)||
                    (std::any_cast<OPERATOR>(raw_expression_[pos])!=OPERATOR::LPAR&&
                    std::any_cast<OPERATOR>(raw_expression_[pos])!=OPERATOR::LBRA&&
                    std::any_cast<OPERATOR>(raw_expression_[pos])!=OPERATOR::CALL)))
                    pos=FA[pos];
                if(pos==-1)
                    throw invalid_expression(R"(unmatched ']')");
                if(std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LPAR||
                    std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::CALL)
                    throw invalid_expression(R"(closing parenthesis ']' does not match opening parenthesis '(')");
                FA[i]=FA[pos],LS[i]=LS[pos],RS[i]=RS[pos];
                if(LS[i]!=-1)
                    FA[LS[i]]=i;
                if(RS[i]!=-1)
                    FA[RS[i]]=i;
                if(FA[pos]!=-1)
                {
                    if(LS[FA[pos]]==pos)
                        LS[FA[pos]]=i;
                    else
                        RS[FA[pos]]=i;
                }
            }
            else if(tmp==OPERATOR::COMMA)
            {
                if(i==l||FA[i]!=-1)
                    throw invalid_expression("invalid syntax");
                int pos=i-1;
                while(FA[pos]!=-1&&priority(std::any_cast<OPERATOR>(raw_expression_[FA[pos]]))>=
                    priority(std::any_cast<OPERATOR>(raw_expression_[i])))
                    pos=FA[pos];
                if(FA[pos]!=-1)
                {
                    if(LS[FA[pos]]==pos)
                        LS[FA[pos]]=i;
                    else
                        RS[FA[pos]]=i;
                }
                FA[i]=FA[pos],FA[pos]=i,LS[i]=pos;
                if(i+1!=r&&(raw_expression_[i+1].type()!=typeid(OPERATOR)||
                    std::any_cast<OPERATOR>(raw_expression_[i+1])!=OPERATOR::RPAR&&
                    std::any_cast<OPERATOR>(raw_expression_[i+1])!=OPERATOR::RBRA))
                    RS[i]=i+1,FA[i+1]=i;
            }
        }
        else if(raw_expression_[i].type()==typeid(KEYWORD)||(i!=l&&FA[i]==-1))
            throw invalid_expression("invalid syntax");
    }
    int now=r-1;
    while(now!=-1)
    {
        if(raw_expression_[now].type()==typeid(OPERATOR)&&
            (std::any_cast<OPERATOR>(raw_expression_[now])==OPERATOR::LPAR||
            std::any_cast<OPERATOR>(raw_expression_[now])==OPERATOR::CALL))
            throw invalid_expression(R"('(' was never closed)");
        if(raw_expression_[now].type()==typeid(OPERATOR)&&
            std::any_cast<OPERATOR>(raw_expression_[now])==OPERATOR::LBRA)
            throw invalid_expression(R"('[' was never closed)");
        now=FA[now];
    }
    int rt=l;
    for(int i=l;i<r;i++)
        if((raw_expression_[i].type()!=typeid(OPERATOR)||
            (std::any_cast<OPERATOR>(raw_expression_[i])!=OPERATOR::LPAR&&
            std::any_cast<OPERATOR>(raw_expression_[i])!=OPERATOR::LBRA&&
            std::any_cast<OPERATOR>(raw_expression_[i])!=OPERATOR::CALL))&FA[i]==-1)
                rt=i;
    return rt;
}
inline int builder()
{
    // std::cerr<<"tree builder"<<std::endl;
    // for(const auto& t:raw_expression_)
    //     print_to_screen(t),std::cerr<<" ";
    // puts("");
    if(raw_expression_.empty())
        return 0;
    LS.clear(),RS.clear(),FA.clear(),RES.clear();
    LS.resize(raw_expression_.size(),-1);
    RS.resize(raw_expression_.size(),-1);
    FA.resize(raw_expression_.size(),-1);
    if(raw_expression_.front().type()==typeid(KEYWORD))
    {
        if(const auto fi=std::any_cast<KEYWORD>(raw_expression_.front());
            fi==KEYWORD::BREAK||fi==KEYWORD::CONTINUE||fi==KEYWORD::PASS)
        {
            if(static_cast<int>(raw_expression_.size())!=1)
                throw invalid_expression("invalid syntax");
            return 0;
        }
        else
        {
            if(fi==KEYWORD::GLOBAL||fi==KEYWORD::NONLOCAL)
            {
                if(static_cast<int>(raw_expression_.size())==1)
                    throw invalid_expression("invalid syntax");
                return LS[0]=builder_arithmetic(1),FA[LS[0]]=0,0;
            }
            if(fi==KEYWORD::IF||fi==KEYWORD::ELIF||fi==KEYWORD::WHILE||fi==KEYWORD::DEF)
            {
                const int pos=find_colon();
                if(pos==-1)
                    throw invalid_expression(R"(expected ':')");
                if(pos==1)
                    throw invalid_expression("invalid syntax");
                LS[0]=builder_arithmetic(1,pos),FA[LS[0]]=0;
                if(pos+1!=static_cast<int>(raw_expression_.size()))
                {
                    if(raw_expression_[pos+1].type()==typeid(KEYWORD))
                    {
                        const auto tmp=std::any_cast<KEYWORD>(raw_expression_[pos+1]);
                        RS[0]=pos+1,FA[RS[0]]=0;
                        if(tmp==KEYWORD::GLOBAL||tmp==KEYWORD::NONLOCAL)
                        {
                            if(pos+2==static_cast<int>(raw_expression_.size()))
                                throw invalid_expression("invalid syntax");
                            LS[pos+1]=builder_arithmetic(pos+2),FA[LS[pos+1]]=pos+1;
                        }
                        else if(tmp==KEYWORD::BREAK||tmp==KEYWORD::CONTINUE||tmp==KEYWORD::PASS)
                        {
                            if(pos+2!=static_cast<int>(raw_expression_.size()))
                                throw invalid_expression("invalid syntax");
                        }
                        else if(tmp==KEYWORD::RETURN)
                        {
                            if(static_cast<int>(raw_expression_.size())!=pos+2)
                                LS[pos+1]=builder_arithmetic(pos+2),FA[LS[pos+1]]=pos+1;
                        }
                        else
                            throw invalid_expression("invalid syntax");
                    }
                    else
                        RS[0]=builder_arithmetic(pos+1),FA[RS[0]]=0;
                }
                return 0;
            }
            if(fi==KEYWORD::ELSE)
            {
                const int pos=find_colon();
                if(pos!=1)
                    throw invalid_expression(R"(expected ':')");
                if(static_cast<int>(raw_expression_.size())!=2)
                {
                    if(raw_expression_[2].type()==typeid(KEYWORD))
                    {
                        const auto tmp=std::any_cast<KEYWORD>(raw_expression_[2]);
                        RS[0]=2,FA[RS[0]]=0;
                        if(tmp==KEYWORD::GLOBAL||tmp==KEYWORD::NONLOCAL)
                        {
                            if(static_cast<int>(raw_expression_.size())==3)
                                throw invalid_expression("invalid syntax");
                            LS[2]=builder_arithmetic(3),FA[LS[2]]=2;
                        }
                        else if(tmp==KEYWORD::BREAK||tmp==KEYWORD::CONTINUE||tmp==KEYWORD::PASS)
                        {
                            if(static_cast<int>(raw_expression_.size())!=3)
                                throw invalid_expression("invalid syntax");
                        }
                        else if(tmp==KEYWORD::RETURN)
                        {
                            if(static_cast<int>(raw_expression_.size())!=3)
                                LS[2]=builder_arithmetic(3),FA[LS[2]]=2;
                        }
                        else
                            throw invalid_expression("invalid syntax");
                    }
                    else
                        LS[0]=builder_arithmetic(pos+1),FA[LS[0]]=0;
                }
                return 0;
            }
            if(fi==KEYWORD::RETURN)
            {
                if(static_cast<int>(raw_expression_.size())!=1)
                    LS[0]=builder_arithmetic(1),FA[LS[0]]=0;
                return 0;
            }
        }
    }
    return builder_arithmetic();
}

/*
 *this part of code is for executor
 */
inline std::any executor_single(const int &rt,const bool type=true);
inline std::any executor_block(const std::vector<std::pair<int,std::vector<std::any>>>&,int,
    const std::vector<DICT_PTR>&);
inline void make_binding(const std::string &,const DICT_PTR &);
inline std::any executor_arithmetic(const int &rt)
{
    std::vector<int> stack;
    stack.push_back(rt);
    while(!stack.empty())
    {
        if(const int x=stack.back(); raw_expression_[x].type()==typeid(KEYWORD))
            throw invalid_expression("invalid syntax");
        else if(LS[x]<0&&RS[x]<0)
        {
            stack.pop_back();
            if(raw_expression_[x].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[x])==OPERATOR::RPAR)
                RES[x]=Tuple();
            else
                RES[x]=get_value(raw_expression_[x]);
        }
        else if(LS[x]>=0&&FA[LS[x]]==x)
            stack.push_back(LS[x]),FA[LS[x]]=-1;
        else if(raw_expression_[x].type()==typeid(OPERATOR)&&
            std::any_cast<OPERATOR>(raw_expression_[x])==OPERATOR::CALL_)
        {
            stack.pop_back();
            auto obj_left=remove_ptr(RES[LS[x]]);
            if(obj_left.type()!=typeid(python_function))
                throw invalid_expression("\'"+type_name(obj_left)+"\' object is not callable");
            auto now_func=std::any_cast<python_function>(obj_left);
            if(now_func.id<0)
            {
                std::vector<int> sta;
                std::vector<std::any> par;
                sta.push_back(RS[x]);
                while(!sta.empty())
                {
                    const int x=sta.back();
                    sta.pop_back();
                    if(x==-1)
                        continue;
                    if(raw_expression_[x].type()==typeid(OPERATOR))
                    {
                        if(const auto tmp=std::any_cast<OPERATOR>(raw_expression_[x]); tmp==OPERATOR::COMMA)
                        {
                            sta.push_back(RS[x]);
                            sta.push_back(LS[x]);
                            continue;
                        }
                        else if(tmp==OPERATOR::ALLO)
                            throw invalid_expression("invalid keyword argument");
                    }
                    par.push_back(remove_ptr(executor_single(x,false)));
                }
                if(now_func.id==-1)
                {
                    if(par.empty())
                        RES[x]=int65536(0);
                    else if(par.size()==1)
                        RES[x]=cast_to_int(par.back());
                    else if(par.size()==2)
                    {
                        if(par[1].type()!=typeid(int65536)&&par[1].type()!=typeid(bool))
                            throw invalid_expression("\'"+type_name(par[1])+R"(' object cannot be interpreted as an integer)");
                        int65536 base=cast_to_int(par[1]);
                        if(!base)
                            base=int65536(10);
                        if(base<int65536(2)||base>int65536(36))
                            throw invalid_expression("int() base must be >= 2 and <= 36, or 0");
                        if(par[0].type()!=typeid(str))
                            throw invalid_expression("int() can't convert non-string with explicit base");
                        const auto& tmp=static_cast<std::string>(std::any_cast<str>(par[0]));
                        int65536 res(0);
                        for(auto t:tmp)
                        {
                            int65536 now(37);
                            if(t>='0'&&t<='9')
                                now=int65536(t-'0');
                            else if(t>='a'&&t<='z')
                                now=int65536(t-'a'+10);
                            else if(t>='A'&&t<='Z')
                                now=int65536(t-'A'+10);
                            if(now>=base)
                                throw invalid_expression("invalid literal for int() with base "+std::string(base));
                            res=res*base+now;
                        }
                        RES[x]=res;
                    }
                    else
                        throw invalid_expression("int() takes at most 2 arguments");
                }
                if(now_func.id==-2)
                {
                    if(par.empty())
                        RES[x]=float2048(0);
                    else if(par.size()==1)
                        RES[x]=cast_to_float(par[0]);
                    else
                        throw invalid_expression("float expected at most 1 argument");
                }
                if(now_func.id==-3)
                {
                    if(par.empty())
                        RES[x]=false;
                    else if(par.size()==1)
                        RES[x]=cast_to_bool(par[0]);
                    else
                        throw invalid_expression("bool expected at most 1 argument");
                }
                else if(now_func.id==-4)
                {
                    if(par.empty())
                        RES[x]=str("");
                    else if(par.size()==1)
                        RES[x]=cast_to_str(par[0]);
                    else
                        throw invalid_expression("str() takes at most 1 argument");
                }
                if(now_func.id==-5)
                {
                    if(par.empty())
                        RES[x]=Tuple();
                    else if(par.size()==1)
                        RES[x]=cast_to_tuple(par[0]);
                    else
                        throw invalid_expression("tuple expected at most 1 argument");
                }
                if(now_func.id==-6)
                {
                    for(int i=0;i<par.size();i++)
                    {
                        if(i)
                            std::cout<<" ";
                        print_to_screen(par[i],true);
                    }
                    std::cout<<std::endl;
                    RES[x]=0ll;
                }
                if(now_func.id==-7)
                {
                    if(par.size()==1)
                        print_to_screen(par[0],true);
                    if(par.size()>1)
                        throw invalid_expression("input expected at most 1 argument");
                    std::string s;
                    getline(std::cin,s);
                    RES[x]=str(s);
                }
                if(now_func.id==-8)
                {
                    if(par.empty())
                        throw invalid_expression("min expected at least 1 argument");
                    if(par.size()==1)
                    {
                        if(par[0].type()!=typeid(Tuple))
                            throw invalid_expression("\'"+type_name(par[0])+R"(' object is not iterable)");
                        const auto tmp=std::any_cast<Tuple>(par[0]);
                        if(!tmp.SIZE())
                            throw invalid_expression("min() iterable argument is empty");
                        RES[x]=tmp[0];
                        for(const auto& t:tmp.val)
                            if(cast_to_bool(operator_::operator_greater(RES[x],t)))
                                RES[x]=t;
                    }
                    else
                    {
                        RES[x]=par[0];
                        for(int i=1;i<par.size();i++)
                            if(cast_to_bool(operator_::operator_greater(RES[x],par[i])))
                                RES[x]=par[i];
                    }
                }
                if(now_func.id==-9)
                {
                    if(par.empty())
                        throw invalid_expression("min expected at least 1 argument");
                    if(par.size()==1)
                    {
                        if(par[0].type()!=typeid(Tuple))
                            throw invalid_expression("\'"+type_name(par[0])+R"(' object is not iterable)");
                        const auto tmp=std::any_cast<Tuple>(par[0]);
                        if(!tmp.SIZE())
                            throw invalid_expression("min() iterable argument is empty");
                        RES[x]=tmp[0];
                        for(const auto& t:tmp.val)
                            if(cast_to_bool(operator_::operator_greater(t,RES[x])))
                                RES[x]=t;
                    }
                    else
                    {
                        RES[x]=par[0];
                        for(int i=1;i<par.size();i++)
                            if(cast_to_bool(operator_::operator_greater(par[i],RES[x])))
                                RES[x]=par[i];
                    }
                }
                if(now_func.id==-10)
                {
                    if(par.empty())
                        throw invalid_expression("abs() takes exactly one argument");
                    if(par.size()==1)
                    {
                        if(par[0].type()!=typeid(int65536)&&par[0].type()!=typeid(bool)&&par[0].type()!=typeid(float2048<>))
                            throw invalid_expression("bad operand type for abs()");
                        if(cast_to_bool(operator_::operator_greater_eq(par[0],int65536(0))))
                            RES[x]=operator_::operator_plus(par[0]);
                        else
                            RES[x]=operator_::operator_minus(par[0]);
                    }
                    else
                        throw invalid_expression("abs() takes exactly one argument");
                }
                if(now_func.id==-11)
                {
                    if(par.empty())
                        throw invalid_expression("sum() takes at least 1 argument");
                    if(par.size()>=3)
                        throw invalid_expression("sum() takes at least 3 arguments");
                    RES[x]=int65536(0);
                    if(par.size()==2)
                        RES[x]=par[1];
                    if(par[0].type()!=typeid(Tuple))
                        throw invalid_expression("\'"+type_name(par[0])+R"(' object is not iterable)");
                    if(RES[x].type()==typeid(str))
                        throw invalid_expression("sum() can't sum strings");
                    for(const auto tmp=std::any_cast<Tuple>(par[0]); const auto &t:tmp.val)
                        RES[x]=operator_::operator_add(RES[x],t);
                }
                continue;
            }
            now_func.call(RS[x]);
            const auto PRE_LOCAL_DICT=LOCAL_DICT,PRE_NONLOCAL_DICT=NONLOCAL_DICT;
            const auto PRE_MY_DICT=MY_DICT;
            const auto PRE_BIND_DICT=BIND_DICT;
            const auto PRE_GLOBAL_STATE=GLOBAL_STATE;
            MY_DICT.clear();
            BIND_DICT.clear();
            LOCAL_DICT=now_func.LOCAL_DICT;
            for(const auto TMP_LOCAL_DICT = *LOCAL_DICT;
                const auto& fi:TMP_LOCAL_DICT|std::views::keys)
                make_binding(fi,LOCAL_DICT);
            NONLOCAL_DICT=now_func.NONLOCAL_DICT;
            const auto LS_=LS,RS_=RS,FA_=FA;
            const auto RAW=raw_expression_,RES_=RES;
            LS.clear(),RS.clear(),FA.clear(),raw_expression_.clear(),RES.clear();
            auto tmp_res=executor_block(now_func.code,true,now_func.PRE_DICT);
            LS=LS_,RS=RS_,FA=FA_,raw_expression_=RAW,RES=RES_;
            RES[x]=tmp_res;
            GLOBAL_STATE=PRE_GLOBAL_STATE;
            LOCAL_DICT=PRE_LOCAL_DICT;
            NONLOCAL_DICT=PRE_NONLOCAL_DICT;
            MY_DICT=PRE_MY_DICT;
            BIND_DICT=PRE_BIND_DICT;
        }
        else if(RS[x]>=0&&FA[RS[x]]==x)
        {
            if(const auto tmp=std::any_cast<OPERATOR>(raw_expression_[x]);
                (tmp==OPERATOR::OR&&cast_to_bool(RES[LS[x]]))||(tmp==OPERATOR::AND&&!cast_to_bool(RES[LS[x]])))
            {
                stack.pop_back();
                RES[x]=RES[LS[x]];
            }
            else
                stack.push_back(RS[x]),FA[RS[x]]=-1;
        }
        else
        {
            stack.pop_back();
            if(const auto tmp=std::any_cast<OPERATOR>(raw_expression_[x]); tmp==OPERATOR::ADD)
                RES[x]=operator_::operator_add(remove_ptr(RES[LS[x]]),remove_ptr(RES[RS[x]]));
            else if(tmp==OPERATOR::SUB)
                RES[x]=operator_::operator_sub(remove_ptr(RES[LS[x]]),remove_ptr(RES[RS[x]]));
            else if(tmp==OPERATOR::MUL)
                RES[x]=operator_::operator_mul(remove_ptr(RES[LS[x]]),remove_ptr(RES[RS[x]]));
            else if(tmp==OPERATOR::DIV)
                RES[x]=operator_::operator_div(remove_ptr(RES[LS[x]]),remove_ptr(RES[RS[x]]));
            else if(tmp==OPERATOR::MOD)
                RES[x]=operator_::operator_mod(remove_ptr(RES[LS[x]]),remove_ptr(RES[RS[x]]));
            else if(tmp==OPERATOR::IDIV)
                RES[x]=operator_::operator_idiv(remove_ptr(RES[LS[x]]),remove_ptr(RES[RS[x]]));
            else if(tmp==OPERATOR::POW)
                RES[x]=operator_::operator_pow(remove_ptr(RES[LS[x]]),remove_ptr(RES[RS[x]]));
            else if(tmp==OPERATOR::OR||tmp==OPERATOR::AND)
                RES[x]=RES[RS[x]];
            else if(tmp==OPERATOR::NOT)
                RES[x]=operator_::operator_not(remove_ptr(RES[LS[x]]));
            else if(tmp==OPERATOR::PLUS)
                RES[x]=operator_::operator_plus(remove_ptr(RES[LS[x]]));
            else if(tmp==OPERATOR::MINUS)
                RES[x]=operator_::operator_minus(remove_ptr(RES[LS[x]]));
            else if(tmp==OPERATOR::RPAR)
                RES[x]=RES[LS[x]];
            else if(tmp==OPERATOR::RBRA)
                RES[x]=operator_::operator_index(remove_ptr(RES[LS[x]]),remove_ptr(RES[RS[x]]));
            else if(tmp==OPERATOR::LE||tmp==OPERATOR::LEQ||tmp==OPERATOR::GE||tmp==OPERATOR::GEQ||tmp==OPERATOR::EQ||
                tmp==OPERATOR::NEQ||tmp==OPERATOR::IS||tmp==OPERATOR::IN||tmp==OPERATOR::IS_NOT||tmp==OPERATOR::NOT_IN)
            {
                auto fi=remove_ptr(RES[LS[x]]);
                if(raw_expression_[LS[x]].type()==typeid(OPERATOR))
                {
                    if(const auto pre=std::any_cast<OPERATOR>(raw_expression_[LS[x]]);
                        pre==OPERATOR::LE||pre==OPERATOR::LEQ||pre==OPERATOR::GE||pre==OPERATOR::GEQ||pre==OPERATOR::EQ||
                        pre==OPERATOR::NEQ||pre==OPERATOR::IS||pre==OPERATOR::IN||pre==OPERATOR::IS_NOT||
                        pre==OPERATOR::NOT_IN)
                        fi=remove_ptr(raw_expression_[RS[LS[x]]]);
                }
                if(tmp==OPERATOR::LE)
                    RES[x]=operator_::operator_less(fi,remove_ptr(RES[RS[x]]));
                if(tmp==OPERATOR::LEQ)
                    RES[x]=operator_::operator_less_eq(fi,remove_ptr(RES[RS[x]]));
                if(tmp==OPERATOR::GE)
                    RES[x]=operator_::operator_greater(fi,remove_ptr(RES[RS[x]]));
                if(tmp==OPERATOR::GEQ)
                    RES[x]=operator_::operator_greater_eq(fi,remove_ptr(RES[RS[x]]));
                if(tmp==OPERATOR::EQ)
                    RES[x]=operator_::operator_equal(fi,remove_ptr(RES[RS[x]]));
                if(tmp==OPERATOR::NEQ)
                    RES[x]=operator_::operator_not_equal(fi,remove_ptr(RES[RS[x]]));
            }
            else if(tmp==OPERATOR::COMMA)
            {
                Tuple lR;
                if(raw_expression_[LS[x]].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[LS[x]])==OPERATOR::COMMA)
                    lR=std::any_cast<Tuple>(remove_ptr(RES[LS[x]]));
                else
                    lR=Tuple(remove_ptr(RES[LS[x]]));
                if(RS[x]==-1)
                    RES[x]=lR;
                else
                {
                    Tuple rR;
                    if(raw_expression_[RS[x]].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[RS[x]])==OPERATOR::COMMA)
                        rR=std::any_cast<Tuple>(remove_ptr(RES[RS[x]]));
                    else
                        rR=Tuple(remove_ptr(RES[RS[x]]));
                    RES[x]=lR+rR;
                }
            }
            else
                throw invalid_expression("invalid syntax");
        }
    }
    return RES[rt];
}
inline void make_binding(const std::string &s,const DICT_PTR &d)
{
    if(!d->contains(s))
        (*d)[s]=std::make_shared<POINTER>(nullptr);
    MY_DICT[s]=d;
}
inline void find_binding(const std::string &s)
{
    if(!BIND_DICT.contains(s))
        BIND_DICT[s]=GLOBAL_STATE?0:2;
    const int x=BIND_DICT[s];
    if(!x)
        make_binding(s,LOCAL_DICT);
    else if(x==1)
        make_binding(s,NONLOCAL_DICT);
    else
        make_binding(s,GLOBAL_DICT);
}
inline POINTER make_obj(const std::any &a)
{
    if(a.type()==typeid(POINTER))
        return std::any_cast<POINTER>(a);
    return std::make_shared<OBJECT>(a);
}
inline void assign(const std::string &s,const POINTER &a)
{
    find_binding(s);
    *(*(MY_DICT[s]))[s]=a;
}
inline std::any packer(const int x)
{
    if(raw_expression_[x].type()==typeid(OPERATOR))
    {
        const auto now=std::any_cast<OPERATOR>(raw_expression_[x]);
        if(now==OPERATOR::COMMA)
        {
            Tuple lR;
            if(raw_expression_[LS[x]].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[LS[x]])==OPERATOR::COMMA)
                lR=std::any_cast<Tuple>(packer(LS[x]));
            else
                lR=Tuple(packer(LS[x]));
            if(RS[x]==-1)
                return lR;
            Tuple rR;
            if(raw_expression_[RS[x]].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[RS[x]])==OPERATOR::COMMA)
                rR=std::any_cast<Tuple>(packer(RS[x]));
            else
                rR=Tuple(packer(RS[x]));
            return lR+rR;
        }
        if(now==OPERATOR::RPAR)
            return packer(LS[x]);
        throw invalid_expression("cannot assign to expression");
    }
    if(raw_expression_[x].type()!=typeid(std::string))
        throw invalid_expression("cannot assign to literal");
    find_binding(std::any_cast<std::string>(raw_expression_[x]));
    return raw_expression_[x];
}
inline void unpacker(const std::any &a,const OBJECT &b)
{
    if(a.type()!=typeid(Tuple))
        return assign(std::any_cast<std::string>(a),make_obj(b));
    if(b.type()!=typeid(Tuple))
        throw invalid_expression("cannot unpack non-iterable int object");
    const auto p=std::any_cast<Tuple>(a),q=std::any_cast<Tuple>(b);
    if(p.SIZE()<q.SIZE())
        throw invalid_expression("too many values to unpack");
    if(p.SIZE()>q.SIZE())
        throw invalid_expression("not enough values to unpack");
    for(int i=0;i<p.SIZE();i++)
        unpacker(p[i],q[i]);
}
inline void executor_assignment(const int &rt)
{
    if(LS[rt]==-1||RS[rt]==-1)
        throw invalid_expression("invalid syntax");
    if(const auto ope=std::any_cast<OPERATOR>(raw_expression_[rt]); ope==OPERATOR::ASSIGN)
    {
        std::vector<int> sta,fd;
        sta.push_back(LS[rt]);
        while(!sta.empty())
        {
            const int x=sta.back();sta.pop_back();
            if(raw_expression_[x].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[x])==OPERATOR::ASSIGN)
                sta.push_back(LS[x]),sta.push_back(RS[x]);
            else
                fd.push_back(x),packer(x);
        }
        const auto tmp=remove_ptr(executor_arithmetic(RS[rt]));
        for(const auto x:fd)
            unpacker(packer(x),tmp);
    }
    else
    {
        const auto lf=packer(LS[rt]);
        const auto tmp=remove_ptr(executor_arithmetic(RS[rt]));
        if(lf.type()==typeid(Tuple))
            throw invalid_expression(R"('tuple' is an illegal expression for augmented assignment)");
        if(lf.type()!=typeid(std::string))
            throw invalid_expression(R"('literal' is an illegal expression for augmented assignment)");\
        if(ope==OPERATOR::ADDASS)
            assign(std::any_cast<std::string>(lf),make_obj(operator_::operator_add(remove_ptr(get_value(lf)),tmp)));
        if(ope==OPERATOR::SUBASS)
            assign(std::any_cast<std::string>(lf),make_obj(operator_::operator_sub(remove_ptr(get_value(lf)),tmp)));
        if(ope==OPERATOR::MULASS)
            assign(std::any_cast<std::string>(lf),make_obj(operator_::operator_mul(remove_ptr(get_value(lf)),tmp)));
        if(ope==OPERATOR::DIVASS)
            assign(std::any_cast<std::string>(lf),make_obj(operator_::operator_div(remove_ptr(get_value(lf)),tmp)));
        if(ope==OPERATOR::MODASS)
            assign(std::any_cast<std::string>(lf),make_obj(operator_::operator_mod(remove_ptr(get_value(lf)),tmp)));
        if(ope==OPERATOR::IDIVASS)
            assign(std::any_cast<std::string>(lf),make_obj(operator_::operator_idiv(remove_ptr(get_value(lf)),tmp)));
        if(ope==OPERATOR::POWASS)
            assign(std::any_cast<std::string>(lf),make_obj(operator_::operator_pow(remove_ptr(get_value(lf)),tmp)));
    }
}
inline std::any executor_single(const int &rt,const bool type)
{
    RES.resize(static_cast<int>(raw_expression_.size()));
    if(raw_expression_[rt].type()==typeid(OPERATOR)&&type)
    {
        const auto ope=std::any_cast<OPERATOR>(raw_expression_[rt]);
        if(ope==OPERATOR::ASSIGN||ope==OPERATOR::ADDASS||ope==OPERATOR::SUBASS||ope==OPERATOR::MULASS||
            ope==OPERATOR::DIVASS||ope==OPERATOR::MODASS||ope==OPERATOR::IDIVASS||ope==OPERATOR::POWASS)
            return executor_assignment(rt),0ll;
    }
    return executor_arithmetic(rt);
}
inline void python_function::call(const int rt)
{
    DICT tmp_empty;
    NONLOCAL_DICT=std::make_shared<DICT>(DICT{});
    for(const auto& p:PRE_DICT)
        for(const auto q=*p;
            const auto& [fi,se]:q)
            (*NONLOCAL_DICT)[fi]=se;
    LOCAL_DICT=std::make_shared<DICT>(DICT{});
    for(const auto& [fi,se]:para_dict)
        (*LOCAL_DICT)[fi]=std::make_shared<POINTER>(make_obj(se));
    std::unordered_map<std::string,bool> chk;
    std::vector<int> sta;
    sta.push_back(rt);
    int op=0,cnt=0;
    while(!sta.empty())
    {
        int x=sta.back();
        sta.pop_back();
        if(x==-1)
            continue;
        if(raw_expression_[x].type()==typeid(OPERATOR))
        {
            if(const auto tmp=std::any_cast<OPERATOR>(raw_expression_[x]); tmp==OPERATOR::COMMA)
            {
                sta.push_back(RS[x]);
                sta.push_back(LS[x]);
                continue;
            }
            else if(tmp==OPERATOR::ALLO)
            {
                op=1;
                if(LS[x]==-1||RS[x]==-1||raw_expression_[LS[x]].type()!=typeid(std::string))
                    throw invalid_expression("invalid syntax");
                const auto name=std::any_cast<std::string>(raw_expression_[LS[x]]);
                if(!is_variable(name))
                    throw invalid_expression("invalid syntax");
                const auto val=executor_arithmetic(RS[x]);
                if(!para_dict.contains(name))
                    throw invalid_expression("unexpected keyword argument \'"+name+"\'");
                if(chk.contains(name))
                    throw invalid_expression("multiple values for argument \'"+name+"\'");
                chk[name]=true;
                (*LOCAL_DICT)[name]=std::make_shared<POINTER>(make_obj(val));
                continue;
            }
        }
        if(op)
            throw invalid_expression("positional argument follows keyword argument");
        if(cnt==parameter.size())
            throw invalid_expression("too much positional arguments");
        const auto name=parameter[cnt++];
        const auto val=executor_single(x,false);
        if(!para_dict.contains(name))
            throw invalid_expression("unexpected keyword argument \'"+name+"\'");
        if(chk.contains(name))
            throw invalid_expression("multiple values for argument \'"+name+"\'");
        chk[name]=true;
        (*LOCAL_DICT)[name]=std::make_shared<POINTER>(make_obj(val));
    }
    for(const auto &t:parameter)
        if((*(*LOCAL_DICT)[t])->type()==typeid(std::string))
            throw invalid_expression("missing argument \'"+t+"\'");
}
inline python_function::python_function(const int rt):id(COUNT_OF_FUNCTION++)
{
    std::vector<int> sta;
    sta.push_back(rt);
    int op=0;
    while(!sta.empty())
    {
        const int x=sta.back();sta.pop_back();
        if(x==-1)
            continue;
        if(raw_expression_[x].type()==typeid(OPERATOR))
        {
            if(const auto tmp=std::any_cast<OPERATOR>(raw_expression_[x]); tmp==OPERATOR::COMMA)
            {
                sta.push_back(RS[x]);
                sta.push_back(LS[x]);
                continue;
            }
            else if(tmp==OPERATOR::ALLO)
            {
                op=1;
                if(LS[x]==-1||RS[x]==-1||raw_expression_[LS[x]].type()!=typeid(std::string))
                    throw invalid_expression("invalid syntax");
                const auto name=std::any_cast<std::string>(raw_expression_[LS[x]]);
                if(!is_variable(name))
                    throw invalid_expression("invalid syntax");
                const auto val=executor_single(RS[x],false);
                parameter.push_back(name);
                if(para_dict.contains(name))
                    throw invalid_expression("duplicate argument \'"+name+"\' in function definition");
                para_dict[name]=val;
                continue;
            }
        }
        if(op)
            throw invalid_expression("parameter without a default follows parameter with a default");
        if(raw_expression_[x].type()!=typeid(std::string))
            throw invalid_expression("invalid syntax");
        const auto name=std::any_cast<std::string>(raw_expression_[x]);
        if(!is_variable(name))
            throw invalid_expression("invalid syntax");
        parameter.push_back(name);
        if(para_dict.contains(name))
            throw invalid_expression("duplicate argument \'"+name+"\' in function definition");
        para_dict[name]=name;
    }
}
inline std::any executor_block(const std::vector<std::pair<int,std::vector<std::any>>> &CODE_BLOCK,const int state,
    const std::vector<DICT_PTR> &LIST={})
{
    GLOBAL_STATE=state;
    std::vector<running_information> running_code;
    running_code.emplace_back(0,-1,RUNNING_STATE::NORMAL);
    int now=0,RETURN=0;
    python_function now_func;
    std::string func_name;
    auto brief_single=[&](int &i,const int rt)->void
    {
        if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::PASS){}
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::CONTINUE)
        {
            while(!running_code.empty()&&running_code.back().pre_state!=RUNNING_STATE::SUCCESSFUL_WHILE)
                running_code.pop_back();
            if(running_code.empty())
                throw invalid_expression(R"('continue' not properly in loop)");
            i=running_code.back().pre_pos-1;
            now=running_code.back().indentation_count;
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::BREAK)
        {
            while(!running_code.empty()&&running_code.back().pre_state!=RUNNING_STATE::SUCCESSFUL_WHILE)
                running_code.pop_back();
            if(running_code.empty())
                throw invalid_expression(R"('break' outside loop)");
            now=running_code.back().indentation_count;
            running_code.back().pre_state=RUNNING_STATE::BROKEN_WHILE;
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::RETURN)
        {
            if(!state)
                throw invalid_expression(R"('return' outside function)");
            RETURN=1;
            if(LS[rt]==-1)
                RES[rt]=0ll;
            else
                RES[rt]=executor_single(LS[rt],true);
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::GLOBAL)
        {
            std::vector<int> vec;
            vec.push_back(LS[rt]);
            while(!vec.empty())
            {
                const auto x=vec.back();vec.pop_back();
                if(x==-1)
                    throw invalid_expression("invalid syntax");
                if(raw_expression_[x].type()==typeid(OPERATOR))
                {
                    const auto now_ope=std::any_cast<OPERATOR>(raw_expression_[x]);
                    if(now_ope==OPERATOR::COMMA)
                        vec.push_back(LS[x]),vec.push_back(RS[x]);
                    else
                        throw invalid_expression("invalid syntax");
                }
                else if(raw_expression_[x].type()==typeid(std::string))
                {
                    const auto now_str=std::any_cast<std::string>(raw_expression_[x]);
                    if(BIND_DICT.contains(now_str))
                    {
                        if(MY_DICT.contains(now_str))
                            throw invalid_expression(R"(name ')"+now_str+R"(' is used before global declaration)");
                        if(BIND_DICT[now_str]==1)
                            throw invalid_expression(R"(name ')"+now_str+R"(' is nonlocal and global)");
                    }
                    else
                        BIND_DICT[now_str]=2;
                }
            }
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::NONLOCAL)
        {
            std::vector<int> vec;
            vec.push_back(LS[rt]);
            while(!vec.empty())
            {
                const auto x=vec.back();vec.pop_back();
                if(x==-1)
                    throw invalid_expression("invalid syntax");
                if(raw_expression_[x].type()==typeid(OPERATOR))
                {
                    const auto now_ope=std::any_cast<OPERATOR>(raw_expression_[x]);
                    if(now_ope==OPERATOR::COMMA)
                        vec.push_back(LS[x]),vec.push_back(RS[x]);
                    else
                        throw invalid_expression("invalid syntax");
                }
                else if(raw_expression_[x].type()==typeid(std::string))
                {
                    const auto now_str=std::any_cast<std::string>(raw_expression_[x]);
                    if(!NONLOCAL_DICT->contains(now_str))
                        throw invalid_expression(R"(no binding for nonlocal ')"+now_str+R"(' found)");
                    if(BIND_DICT.contains(now_str))
                    {
                        if(MY_DICT.contains(now_str))
                            throw invalid_expression(R"(name ')"+now_str+R"(' is used before nonlocal declaration)");
                        if(BIND_DICT[now_str]==2)
                            throw invalid_expression(R"(name ')"+now_str+R"(' is nonlocal and global)");
                    }
                    else
                        BIND_DICT[now_str]=1;
                }
            }
        }
        else
        {
            if(const auto tmp_res=remove_ptr(executor_single(rt)); tmp_res.type()!=typeid(long long)&&
                !state&&INTERACTIVE_MODE)
                print_to_screen(tmp_res),std::cout<<std::endl;
            running_code.back()=running_information(now,i,RUNNING_STATE::NORMAL);
        }
    };
    for(int i=0;i<=static_cast<int>(CODE_BLOCK.size());i++)
    {
        const auto x=i==CODE_BLOCK.size()?0:CODE_BLOCK[i].first;
        const auto t=i==CODE_BLOCK.size()?std::vector<std::any>():CODE_BLOCK[i].second;
        if(running_code.back().pre_state==RUNNING_STATE::FUNCTION_CONTENT&&x>=running_code.back().indentation_count)
        {
            now_func.code.emplace_back(x-running_code.back().indentation_count,t);
            assign(func_name,make_obj(now_func));
            continue;
        }
        if(now<x)
            continue;
        int op=0;
        if(running_code.back().pre_state==RUNNING_STATE::SUCCESSFUL_WHILE)
        {
            i=running_code.back().pre_pos-1;
            running_code.back().pre_state=RUNNING_STATE::NORMAL;
            continue;
        }
        while(!running_code.empty()&&running_code.back().indentation_count!=x)
        {
            running_code.pop_back();
            if(running_code.empty())
                throw indentation_error("unexpected indent");
            if(running_code.back().pre_state==RUNNING_STATE::SUCCESSFUL_WHILE)
            {
                i=running_code.back().pre_pos-1;
                running_code.back().pre_state=RUNNING_STATE::NORMAL;
                op=1;
                break;
            }
        }
        if(op||t.empty())
            continue;
        if(x<now)
            now=x;
        raw_expression_=t;
        const int rt=builder();
        // std::vector<int> sta;
        // sta.push_back(rt);
        // std::cerr<<"Tree"<<std::endl;
        // while(!sta.empty())
        // {
        //     const int x=sta.back();sta.pop_back();
        //     print_to_screen(raw_expression_[x]);
        //     std::cerr<<" "<<x<<" "<<LS[x]<<" "<<RS[x]<<" "<<FA[x]<<std::endl;
        //     if(RS[x]!=-1)
        //         sta.push_back(RS[x]);
        //     if(LS[x]!=-1)
        //         sta.push_back(LS[x]);
        // }
        // std::cerr<<"end of tree"<<std::endl;
        if(raw_expression_[rt].type()==typeid(KEYWORD)&&std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::IF)
        {
            const bool check=cast_to_bool(remove_ptr(executor_single(LS[rt],false)));
            if(RS[rt]!=-1)
            {
                if(check)
                {
                    running_code.back()=running_information(x,i,RUNNING_STATE::SUCCESSFUL_IF);
                    brief_single(i,RS[rt]);
                }
                else
                    running_code.back()=running_information(x,i,RUNNING_STATE::FAILED_IF);
            }
            if(RS[rt]==-1)
            {
                if(i+1==CODE_BLOCK.size()||CODE_BLOCK[i+1].first<=now)
                    throw invalid_expression(R"(expected an indented block after 'if' statement)");
                if(check)
                    running_code.back()=running_information(x,i,RUNNING_STATE::SUCCESSFUL_IF),now=CODE_BLOCK[i+1].first;
                else
                    running_code.back()=running_information(x,i,RUNNING_STATE::FAILED_IF);
                running_code.emplace_back(CODE_BLOCK[i+1].first,i+1,RUNNING_STATE::NORMAL);
            }
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::ELIF)
        {
            const auto pre_state=running_code.back().pre_state;
            if(pre_state!=RUNNING_STATE::SUCCESSFUL_IF&&pre_state!=RUNNING_STATE::FAILED_IF&&
                pre_state!=RUNNING_STATE::SUCCESSFUL_ELIF&&pre_state!=RUNNING_STATE::FAILED_ELIF)
                throw invalid_expression("invalid syntax");
            if(pre_state==RUNNING_STATE::FAILED_IF||pre_state==RUNNING_STATE::FAILED_ELIF)
            {
                const bool check=cast_to_bool(remove_ptr(executor_single(LS[rt],false)));
                if(RS[rt]!=-1)
                {
                    if(check)
                    {
                        running_code.back()=running_information(x,i,RUNNING_STATE::SUCCESSFUL_ELIF);
                        brief_single(i,RS[rt]);
                    }
                    else
                        running_code.back()=running_information(x,i,RUNNING_STATE::FAILED_ELIF);
                }
                if(RS[rt]==-1)
                {
                    if(i+1==CODE_BLOCK.size()||CODE_BLOCK[i+1].first<=now)
                        throw invalid_expression(R"(expected an indented block after 'elif' statement)");
                    if(check)
                        running_code.back()=running_information(x,i,RUNNING_STATE::SUCCESSFUL_ELIF),now=CODE_BLOCK[i+1].first;
                    else
                        running_code.back()=running_information(x,i,RUNNING_STATE::FAILED_ELIF);
                    running_code.emplace_back(CODE_BLOCK[i+1].first,i+1,RUNNING_STATE::NORMAL);
                }
            }
            else
            {
                running_code.back()=running_information(now,i,RUNNING_STATE::SUCCESSFUL_ELIF);
                if(RS[rt]==-1)
                {
                    if(i+1==CODE_BLOCK.size()||CODE_BLOCK[i+1].first<=now)
                        throw invalid_expression(R"(expected an indented block after 'elif' statement)");
                    running_code.emplace_back(CODE_BLOCK[i+1].first,i+1,RUNNING_STATE::NORMAL);
                }
            }
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::ELSE)
        {
            const auto pre_state=running_code.back().pre_state;
            if(pre_state!=RUNNING_STATE::SUCCESSFUL_IF&&pre_state!=RUNNING_STATE::FAILED_IF&&
                pre_state!=RUNNING_STATE::SUCCESSFUL_ELIF&&pre_state!=RUNNING_STATE::FAILED_ELIF&&
                pre_state!=RUNNING_STATE::FAILED_WHILE&&pre_state!=RUNNING_STATE::BROKEN_WHILE)
                throw invalid_expression("invalid syntax");
            if(pre_state==RUNNING_STATE::FAILED_IF||pre_state==RUNNING_STATE::FAILED_ELIF||
                pre_state==RUNNING_STATE::FAILED_WHILE)
            {
                if(LS[rt]!=-1)
                {
                    running_code.back()=running_information(x,i,RUNNING_STATE::NORMAL);
                    brief_single(i,LS[rt]);
                }
                if(LS[rt]==-1)
                {
                    if(i+1==CODE_BLOCK.size()||CODE_BLOCK[i+1].first<=now)
                        throw invalid_expression(R"(expected an indented block after 'else' statement)");
                    running_code.back()=running_information(x,i,RUNNING_STATE::NORMAL),now=CODE_BLOCK[i+1].first;
                    running_code.emplace_back(CODE_BLOCK[i+1].first,i+1,RUNNING_STATE::NORMAL);
                }
            }
            else
            {
                running_code.back()=running_information(now,i,RUNNING_STATE::NORMAL);
                if(LS[rt]==-1)
                {
                    if(i+1==CODE_BLOCK.size()||CODE_BLOCK[i+1].first<=now)
                        throw invalid_expression(R"(expected an indented block after 'else' statement)");
                    running_code.emplace_back(CODE_BLOCK[i+1].first,i+1,RUNNING_STATE::NORMAL);
                }
            }
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::WHILE)
        {
            const bool check=cast_to_bool(remove_ptr(executor_single(LS[rt],false)));
            if(RS[rt]!=-1)
            {
                if(check)
                {
                    running_code.back()=running_information(x,i,RUNNING_STATE::SUCCESSFUL_WHILE);
                    brief_single(i,RS[rt]);
                }
                else
                    running_code.back()=running_information(x,i,RUNNING_STATE::FAILED_WHILE);
            }
            if(RS[rt]==-1)
            {
                if(i+1==CODE_BLOCK.size()||CODE_BLOCK[i+1].first<=now)
                    throw invalid_expression(R"(expected an indented block after 'while' statement)");
                if(check)
                    running_code.back()=running_information(x,i,RUNNING_STATE::SUCCESSFUL_WHILE),now=CODE_BLOCK[i+1].first;
                else
                    running_code.back()=running_information(x,i,RUNNING_STATE::FAILED_WHILE);
                running_code.emplace_back(CODE_BLOCK[i+1].first,i+1,RUNNING_STATE::NORMAL);
            }
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::DEF)
        {
            if(LS[rt]==-1||raw_expression_[LS[rt]].type()!=typeid(OPERATOR)||
                std::any_cast<OPERATOR>(raw_expression_[LS[rt]])!=OPERATOR::CALL_)
                throw invalid_expression("invalid syntax");
            if(LS[LS[rt]]==-1||raw_expression_[LS[LS[rt]]].type()!=typeid(std::string))
                throw invalid_expression("invalid syntax");
            const auto name=std::any_cast<std::string>(raw_expression_[LS[LS[rt]]]);
            now_func=python_function(RS[LS[rt]]);
            now_func.PRE_DICT=LIST;
            now_func.PRE_DICT.push_back(LOCAL_DICT);
            if(RS[rt]!=-1)
            {
                now_func.code.emplace_back(0,std::vector<std::any>());
                for(int j=find_colon()+1;j<static_cast<int>(raw_expression_.size());j++)
                    now_func.code.back().second.push_back(raw_expression_[j]);
                running_code.emplace_back(x,i,RUNNING_STATE::NORMAL);
            }
            else
            {
                running_code.emplace_back(x,i,RUNNING_STATE::NORMAL);
                if(i+1==CODE_BLOCK.size()||CODE_BLOCK[i+1].first<=now)
                    throw invalid_expression(R"(expected an indented block after 'def' statement)");
                running_code.emplace_back(CODE_BLOCK[i+1].first,i+1,RUNNING_STATE::FUNCTION_CONTENT);
            }
            assign(name,make_obj(now_func));
            func_name=name;
        }
        else
        {
            brief_single(i,rt);
            if(RETURN)
                return RES[rt];
        }
    }
    return 0ll;
}

/*
 *this part of code is for interpreter
 */
inline int interpreter(const std::string &s)
{
    static std::vector<std::pair<int,std::vector<std::any>>> CODE_BLOCK;
    static int state=0,ok=1;
    static int initialize=0;
    if(!initialize)
    {
        initialize=1;
        python_function tmp;
        tmp.id=-1;
        assign("int",make_obj(tmp));
        tmp.id=-2;
        assign("float",make_obj(tmp));
        tmp.id=-3;
        assign("bool",make_obj(tmp));
        tmp.id=-4;
        assign("str",make_obj(tmp));
        tmp.id=-5;
        assign("tuple",make_obj(tmp));
        tmp.id=-6;
        assign("print",make_obj(tmp));
        tmp.id=-7;
        assign("input",make_obj(tmp));
        tmp.id=-8;
        assign("min",make_obj(tmp));
        tmp.id=-9;
        assign("max",make_obj(tmp));
        tmp.id=-10;
        assign("abs",make_obj(tmp));
        tmp.id=-11;
        assign("sum",make_obj(tmp));
    }
    try
    {
        if(s=="begin")
            state=1;
        else if(s=="end")
        {
            if(!ok)
            {
                if(IN_EOF)
                    mode=0,throw invalid_expression("unexpected EOF while parsing");
                translator("",true);
            }
            state=-1;
        }
        else if(!remove_blank(s).empty()&&translator(s))
        {
            processor();
            CODE_BLOCK.emplace_back(INDENTATION_COUNT,raw_expression_);
            raw_expression_.clear();
            ok=1;
        }
        else
            ok=0;
        if(state==-1||(state==0&&ok))
        {
            state=0;
            LOCAL_DICT=std::make_shared<DICT>(DICT{});
            executor_block(CODE_BLOCK,false);
            CODE_BLOCK.clear();
            return 0;
        }
        return 1;
    }
    catch(invalid_expression &a)
    {
        CODE_BLOCK.clear(),state=0,ok=1;
        throw invalid_expression(a.what());
    }
    catch(std::runtime_error &a)
    {
        CODE_BLOCK.clear(),state=0,ok=1;
        throw std::runtime_error(a.what());
    }
    catch(indentation_error &a)
    {
        CODE_BLOCK.clear(),state=0,ok=1;
        throw indentation_error(a.what());
    }
    catch(std::overflow_error &a)
    {
        CODE_BLOCK.clear(),state=0,ok=1;
        throw std::overflow_error(a.what());
    }
    catch(undefined_behavior &a)
    {
        CODE_BLOCK.clear(),state=0,ok=1;
        throw undefined_behavior(a.what());
    }
}

#endif