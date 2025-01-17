#ifndef TYPE_H
#define TYPE_H

#include <utility>
#include<vector>
#include<any>
#include"int2048.h"
#include"float2048.h"
#include"Complex.h"
#include"exception.h"

using OBJECT=std::any;
using POINTER=std::shared_ptr<OBJECT>;
using DICT=std::unordered_map<std::string,POINTER>;
using DICT_PTR=std::shared_ptr<DICT>;

using _int=int65536;
using _float=float2048<>;
using _complex=Complex<_float>;

inline _complex operator^(_complex &a,_complex &b)
{
    const _float r=sqrt(a.re()*a.re()+a.im()*a.im());
    _float arg;
    if(!a.re())
    {
        if(a.im().sgn())
            arg=calc_pi<6>()/_float(2);
        else
            arg=-calc_pi<6>()/_float(2);
    }
    else if(a.re().sgn())
        arg=atan<6>(a.im()/a.re());
    else if(!a.im()||a.im().sgn())
        arg=atan<6>(a.im()/a.re())+calc_pi<6>();
    else
        arg=atan<6>(a.im()/a.re())-calc_pi<6>();
    const _float n_r=(r^b.re())*exp(-b.im()*arg),n_arg=b.re()*arg+b.im()*ln(r);
    return {n_r*cos(n_arg),n_r*sin(n_arg)};
}

class Tuple;
class Tuple_iterator
{
    int pos;
    POINTER obj;
public:
    explicit Tuple_iterator(const POINTER &a){obj=a;pos=0;}
    POINTER __next__();
};
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
    std::vector<POINTER> val;
    Tuple()=default;
    explicit Tuple(const POINTER &a){val.push_back(a);}
    [[nodiscard]] int SIZE()const{return static_cast<int>(val.size());}
    [[nodiscard]] POINTER __getitem__(const int65536 &a)const;
    [[nodiscard]] POINTER __getitem__(const int &a)const;
    explicit operator bool()const{return !val.empty();}
};

class str;
class str_ascii_iterator
{
    int pos;
    POINTER obj;
public:
    explicit str_ascii_iterator(const POINTER &a){obj=a;pos=0;}
    POINTER __next__();
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
    friend std::strong_ordering operator<=>(const str &a,const str &b){return a.val<=>b.val;}
    friend bool operator==(const str &a,const str &b){return a.val==b.val;}
public:
    std::string val;
    str()=default;
    explicit str(const char a){val.push_back(a);}
    explicit str(std::string a):val(std::move(a)){}
    [[nodiscard]] int SIZE()const{return static_cast<int>(val.size());}
    [[nodiscard]] POINTER __getitem__(const int65536 &a)const;
    [[nodiscard]] POINTER __getitem__(const int &a)const;
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
    std::unordered_map<std::string,POINTER> para_dict;//the parameter dict of itself
    DICT_PTR NONLOCAL_DICT=nullptr,LOCAL_DICT=nullptr;//the nonlocal and local dict of itself
    std::vector<DICT_PTR> PRE_DICT;
    python_function()=default;
    explicit python_function(int);//to build its parameter table and dict
    void call(int);//to build its local dict(parameter dict)
};

class NoneType{};
class ErrorType{};

enum class KEYWORD
{
    DEF,RETURN,BREAK,CONTINUE,PASS,IF,ELIF,ELSE,WHILE,GLOBAL,NONLOCAL,
    ILLEGAL
};
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
inline bool is_operator(const char s)
{
    return s=='+'||s=='-'||s=='*'||s=='/'||s=='%'||
        s=='<'||s=='>'||s=='='||s=='!'||s=='('||s==')'||s=='['||s==']'||
            s==','||s=='.'||s==':'||s=='}';
}

enum class TYPE
{
    Type,Int,Float,Bool,Complex,Str,Tuple,List,Dict,Tuple_iterator,Str_ascii_iterator,Function,
    Nonetype,Builtin_function_and_method,Errortype
};
inline std::string type_what(const TYPE &a)
{
    switch(a)
    {
        case TYPE::Type:return "type";
        case TYPE::Int:return "int";
        case TYPE::Float:return "float";
        case TYPE::Bool:return "bool";
        case TYPE::Complex:return "complex";
        case TYPE::Str:return "str";
        case TYPE::Tuple:return "tuple";
        case TYPE::List:return "list";
        case TYPE::Dict:return "dict";
        case TYPE::Tuple_iterator:return "tuple_iterator";
        case TYPE::Str_ascii_iterator:return "str_ascii_iterator";
        case TYPE::Function:return "function";
        case TYPE::Nonetype:return "NoneType";
        case TYPE::Builtin_function_and_method:return "builtin_function_or_method";
        case TYPE::Errortype:return "ErrorType";
        default:return "UNKNOWN";
    }
}

enum class BUILTIN_FUNCTION_OR_METHOD
{
    PRINT,INPUT,MIN,MAX,ABS,SUM,EVAL,ITER,NEXT,ERROR
};
inline std::string builtin_function_or_method_what(const BUILTIN_FUNCTION_OR_METHOD &a)
{
    switch(a)
    {
        case BUILTIN_FUNCTION_OR_METHOD::PRINT:return "print";
        case BUILTIN_FUNCTION_OR_METHOD::INPUT:return "input";
        case BUILTIN_FUNCTION_OR_METHOD::MIN:return "min";
        case BUILTIN_FUNCTION_OR_METHOD::MAX:return "max";
        case BUILTIN_FUNCTION_OR_METHOD::ABS:return "abs";
        case BUILTIN_FUNCTION_OR_METHOD::SUM:return "sum";
        case BUILTIN_FUNCTION_OR_METHOD::EVAL:return "eval";
        case BUILTIN_FUNCTION_OR_METHOD::ITER:return "iter";
        case BUILTIN_FUNCTION_OR_METHOD::NEXT:return "next";
        default:return "error";
    }
}

enum class RUNNING_STATE
{
    NORMAL,SUCCESSFUL_IF,FAILED_IF,SUCCESSFUL_ELIF,FAILED_ELIF,
    SUCCESSFUL_WHILE,FAILED_WHILE,BROKEN_WHILE,FUNCTION_CONTENT
};

#endif