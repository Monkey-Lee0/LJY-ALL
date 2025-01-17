#ifndef PARSER_H
#define PARSER_H

#include"intelligent.h"
#include<any>
#include<unordered_map>
#include<memory>
#include<iostream>
#include"type.h"

/*
 *code-translator-processor-builder-> expression tree(executable structure)
 *a series of expression tree-code_block executor-single executor-partial executor->result
 */
inline int translator(const std::string &s,bool option=false);
inline void processor();
inline POINTER executor_single(const int &rt,bool type=true);
inline int builder();

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
            throw IndentationError("prohibited use of tabs as indentation");
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
constexpr std::string TYPENAME[]={"int","float","bool","str","tuple","none","function"};

struct running_information
{
    int indentation_count,pre_pos;
    RUNNING_STATE pre_state;
    running_information(const int &a,const int &b,const RUNNING_STATE &c):indentation_count(a),pre_pos(b),pre_state(c){}
};

/*
 *This part of code is for objects and variables
 */
inline DICT_PTR GLOBAL_DICT=std::make_shared<DICT>(DICT{}),NONLOCAL_DICT,LOCAL_DICT;
inline std::unordered_map<std::string,DICT_PTR> MY_DICT;
inline std::unordered_map<std::string,int> BIND_DICT;
inline std::vector<std::any> raw_expression_;
inline std::vector<int> LS,RS,FA;
inline std::vector<POINTER> RES;
inline int GLOBAL_STATE=false,INTERACTIVE_MODE=false;

const std::vector<std::string> MY_KEY=
{
    "def","return","break","continue","pass","if","elif","else","while","global","nonlocal"
};
//It is a function to get the value of variable(get PTR) or literal(get OBJ).
inline POINTER make_ptr(const OBJECT &a){return std::make_shared<OBJECT>(a);}
template<class T> T& obj_cast(const POINTER &a){return *std::any_cast<T>(a.get());}
inline POINTER get_ptr(const std::any &a)
{
    if(a.type()!=typeid(std::string))
        return make_ptr(a);
    const auto s=std::any_cast<std::string>(a);
    if(BIND_DICT.contains(s))
    {
        if(const auto x=BIND_DICT[s]; !x)
            MY_DICT[s]=LOCAL_DICT;
        else if(x==1)
            MY_DICT[s]=NONLOCAL_DICT;
        else
            MY_DICT[s]=GLOBAL_DICT;
    }
    if(MY_DICT.contains(s))
    {
        const auto tmp=MY_DICT[s];
        const auto res=(*tmp)[s];
        if(res==nullptr)
            throw NameError("name \'"+s+"\' is not defined");
        return res;
    }
    if(NONLOCAL_DICT!=nullptr&&NONLOCAL_DICT->contains(s))
        if(const auto res=(*NONLOCAL_DICT)[s]; res!=nullptr)
            return res;
    if(!GLOBAL_DICT->contains(s))
        throw NameError("name \'"+s+"\' is not defined");
    const auto tmp=(*GLOBAL_DICT)[s];
    if(tmp==nullptr)
        throw NameError("name \'"+s+"\' is not defined");
    return tmp;
}

/*
 *This part of code is for operators
 */
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

inline std::string type_name(const POINTER &x)
{
    if(x->type()==typeid(_int))
        return "int";
    if(x->type()==typeid(_float))
        return "float";
    if(x->type()==typeid(_complex))
        return "complex";
    if(x->type()==typeid(bool))
        return "bool";
    if(x->type()==typeid(str))
        return "str";
    if(x->type()==typeid(Tuple))
        return "tuple";
    if(x->type()==typeid(python_function))
        return "function";
    if(x->type()==typeid(NoneType))
        return "NoneType";
    if(x->type()==typeid(Tuple_iterator))
        return "Tuple_iterator";
    if(x->type()==typeid(str_ascii_iterator))
        return "str_ascii_iterator";
    if(x->type()==typeid(TYPE))
        return "type";
    if(x->type()==typeid(BUILTIN_FUNCTION_OR_METHOD))
        return "builtin_function_or_method";
    return "unknown";
}
inline bool is_type_number(const POINTER &x){return x->type()==typeid(_int)||x->type()==typeid(_float)||
    x->type()==typeid(bool)||x->type()==typeid(_complex);}
inline bool is_type_changeable(const POINTER &x){return false;}

inline POINTER Tuple::__getitem__(const int &a)const
{
    if(a>=0)
    {
        if(a>=SIZE())
            throw IndexError("tuple index out of range");
        return val[a];
    }
    if(a<-SIZE())
        throw IndexError("tuple index out of range");
    return val[SIZE()+a];
}
inline POINTER Tuple::__getitem__(const _int &a)const{return __getitem__(static_cast<int>(a));}
inline POINTER Tuple_iterator::__next__()
{
    const auto tmp=obj_cast<Tuple>(obj);
    if(pos==tmp.val.size())
        throw StopIteration("");
    return tmp.val[pos++];
}

inline POINTER str::__getitem__(const int &a)const
{
    if(a>=0)
    {
        if(a>=SIZE())
            throw IndexError("tuple index out of range");
        return make_ptr(str(val[a]));
    }
    if(a<-SIZE())
        throw IndexError("tuple index out of range");
    return make_ptr(str(val[SIZE()+a]));
}
inline POINTER str::__getitem__(const _int &a)const{return __getitem__(static_cast<int>(a));}
inline POINTER str_ascii_iterator::__next__()
{
    const auto tmp=obj_cast<str>(obj);
    if(pos==tmp.val.size())
        throw StopIteration("");
    return make_ptr(str(tmp.val[pos++]));
}

namespace inner_scope
{
    inline POINTER __int__(const POINTER &a=make_ptr(_int(0)),const POINTER &b=make_ptr(_int(10)),const bool ex=false)
    {
        if(ex)
        {
            _int base;
            if(b->type()==typeid(_int))
                base=obj_cast<_int>(b);
            else if(b->type()==typeid(bool))
                base=static_cast<_int>(obj_cast<bool>(b));
            else
                throw TypeError("\'"+type_name(b)+R"(' object cannot be interpreted as an integer)");
            if((base&&base<_int(2))||base>_int(36))
                throw ValueError("int() base must be >= 2 and <= 36, or 0");
            const auto pre_base=base;
            if(a->type()!=typeid(str))
                throw TypeError("int() can't convert non-string with explicit base");
            const auto& tmp=static_cast<std::string>(obj_cast<str>(a));
            int start=0,sign=1;
            if(tmp[0]=='+')
                start++;
            if(tmp[0]=='-')
                start++,sign=0;
            if(tmp.size()==start)
                throw ValueError("invalid literal for int() with base "+std::string(pre_base));
            if(!base)
            {
                if(tmp[start]!='0')
                    base=_int(10);
                else
                {
                    if(tmp.size()==start+1)
                        throw ValueError("invalid literal for int() with base "+std::string(pre_base));
                    if(tmp[start+1]=='b'||tmp[start+1]=='B')
                        base=_int(2);
                    else if(tmp[start+1]=='o'||tmp[start+1]=='O')
                        base=_int(8);
                    else if(tmp[start+1]=='x'||tmp[start+1]=='X')
                        base=_int(16);
                    else
                        throw ValueError("invalid literal for int() with base "+std::string(pre_base));
                }
            }
            if(tmp.size()>=2+start&&tmp[start]=='0')
            {
                if(base==_int(2)&&(tmp[start+1]=='b'||tmp[start+1]=='B'))
                    start+=2;
                if(base==_int(8)&&(tmp[start+1]=='o'||tmp[start+1]=='O'))
                    start+=2;
                if(base==_int(16)&&(tmp[start+1]=='x'||tmp[start+1]=='X'))
                    start+=2;
            }
            _int res(0);
            for(int i=start;i<static_cast<int>(tmp.size());++i)
            {
                const auto t=tmp[i];
                _int now(37);
                if(t>='0'&&t<='9')
                    now=_int(t-'0');
                else if(t>='a'&&t<='z')
                    now=_int(t-'a'+10);
                else if(t>='A'&&t<='Z')
                    now=_int(t-'A'+10);
                if(now>=base)
                    throw ValueError("invalid literal for int() with base "+std::string(pre_base));
                res=res*base+now;
            }
            if(!sign)
                res=-res;
            return make_ptr(res);
        }
        if(a->type()==typeid(_float))
            return make_ptr(_int(obj_cast<_float>(a)));
        if(a->type()==typeid(bool))
            return make_ptr(_int(obj_cast<bool>(a)?1:0));
        if(a->type()==typeid(str))
        {
            try
            {
                return make_ptr(_int(static_cast<std::string>(obj_cast<str>(a))));
            }
            catch(...)
            {
                throw ValueError("invalid literal for int() with base 10");
            }
        }
        if(a->type()==typeid(_int))
            return make_ptr(obj_cast<_int>(a));
        throw TypeError("int() argument must be a string, a bytes-like object or a real number, not '"+type_name(a)+"'");
    }
    inline POINTER __float__(const POINTER &a=make_ptr(_float(0)))
    {
        if(a->type()==typeid(_int))
            return make_ptr(_float(obj_cast<_int>(a)));
        if(a->type()==typeid(bool))
            return make_ptr(_float(obj_cast<bool>(a)?1:0));
        if(a->type()==typeid(str))
        {
            try
            {
                return make_ptr(_float(static_cast<std::string>(obj_cast<str>(a))));
            }
            catch(...)
            {
                throw ValueError("could not convert string to float");
            }
        }
        if(a->type()==typeid(_float))
            return make_ptr(obj_cast<_float>(a));
        throw TypeError("float() argument must be a string or a real number, not '"+type_name(a)+"'");
    }
    inline POINTER __Complex__(const POINTER &a=make_ptr(_float(0)))
    {
        if(a->type()==typeid(_complex))
            return a;
        if(is_type_number(a))
            return make_ptr(_complex(obj_cast<_float>(__float__(a))));
        if(a->type()==typeid(str))
        {
            const auto tmp=static_cast<std::string>(obj_cast<str>(a));
            try
            {
                if(tmp.back()!='j')
                    return make_ptr(_complex(_float(tmp.substr(0,tmp.size()))));
                int pos=-1;
                for(int i=0;i<static_cast<int>(tmp.size());i++)
                    if(tmp[i]=='+')
                    {
                        if(pos!=-1)
                            throw ValueError("");
                        pos=i;
                    }
                if(pos==0||pos+2==tmp.size())
                    throw ValueError("");
                _float left(0),right(0);
                if(pos!=-1)
                    left=_float(tmp.substr(0,pos));
                right=_float(tmp.substr(pos+1,tmp.size()-pos-2));
                return make_ptr(_complex(left,right));
            }
            catch(...){throw ValueError("complex() arg is a malformed string");}
        }
        throw TypeError("complex() first argument must be a string or a number, not '"+type_name(a)+"'");
    }
    inline POINTER __Complex__(const POINTER &a,const POINTER &b)
    {
        if(a->type()==typeid(str))
            throw TypeError("complex() can't take second arg if first is a string");
        if(!is_type_number(a))
            throw TypeError("complex() first argument must be a string or a number");
        if(b->type()==typeid(str))
            throw TypeError("complex() second arg can't be a string");
        if(!is_type_number(b))
            throw TypeError("complex() second argument must be a string or a number, not '"+type_name(a)+"'");
        return make_ptr(obj_cast<_complex>(__Complex__(a))+
            obj_cast<_complex>(__Complex__(b))*_complex(_float(0),_float(1)));
    }
    inline POINTER __bool__(const POINTER &a=make_ptr(false))
    {
        if(a->type()==typeid(_float))
            return make_ptr(static_cast<bool>(obj_cast<_float>(a)));
        if(a->type()==typeid(_int))
            return make_ptr(static_cast<bool>(obj_cast<_int>(a)));
        if(a->type()==typeid(_complex))
            return make_ptr(static_cast<bool>(obj_cast<_complex>(a)));
        if(a->type()==typeid(str))
            return make_ptr(!static_cast<std::string>(obj_cast<str>(a)).empty());
        if(a->type()==typeid(Tuple))
            return make_ptr(!!obj_cast<Tuple>(a).SIZE());
        if(a->type()==typeid(NoneType))
            return make_ptr(false);
        if(a->type()==typeid(bool))
            return make_ptr(obj_cast<bool>(a));
        return make_ptr(true);
    }
    inline POINTER __str__(const POINTER &a=make_ptr(str("")))
    {
        if(a->type()==typeid(_int))
            return make_ptr(str(std::string(obj_cast<_int>(a))));
        if(a->type()==typeid(_float))
        {
            const std::stringstream ss;
            std::streambuf* buffer=std::cout.rdbuf();
            std::cout.rdbuf(ss.rdbuf());
            std::cout<<std::setprecision(12)<<obj_cast<_float>(a);
            const std::string res(ss.str());
            std::cout.rdbuf(buffer);
            return make_ptr(str(res));
        }
        if(a->type()==typeid(_complex))
        {
            auto tmp=obj_cast<_complex>(a);
            std::string res;
            if(tmp.re())
            {
                res+="("+static_cast<std::string>(obj_cast<str>(__str__(make_ptr(tmp.re()))));
                if(tmp.im().sgn())
                    res+="+";
            }
            res+=static_cast<std::string>(obj_cast<str>(__str__(make_ptr(tmp.im()))))+"j";
            if(tmp.re())
                res+=")";
            return make_ptr(str(res));
        }
        if(a->type()==typeid(bool))
            return make_ptr(str(obj_cast<bool>(a)?"True":"False"));
        if(a->type()==typeid(Tuple))
        {
            const auto tpl=obj_cast<Tuple>(a);
            str res;
            res+=str("(");
            if(tpl.SIZE())
            {
                res+=obj_cast<str>(__str__(tpl.__getitem__(0)));
                for(int i=1;i<tpl.SIZE();i++)
                    res+=str(", "),res+=obj_cast<str>(__str__(tpl.__getitem__(i)));
                if(tpl.SIZE()==1)
                    res+=str(",");
            }
            res+=str(")");
            return make_ptr(res);
        }
        if(a->type()==typeid(int))
            return make_ptr(str("operator"+std::to_string(obj_cast<int>(a))));
        if(a->type()==typeid(python_function))
            return make_ptr(str("python_function"+std::to_string(obj_cast<python_function>(a).id)));
        if(a->type()==typeid(NoneType))
            return make_ptr(str("None"));
        if(a->type()==typeid(str))
            return make_ptr(obj_cast<str>(a));
        if(a->type()==typeid(Tuple_iterator))
            return make_ptr(str("Tuple_iterator"));
        if(a->type()==typeid(TYPE))
            return make_ptr(str("<class '"+type_what(obj_cast<TYPE>(a))+"'>"));
        if(a->type()==typeid(BUILTIN_FUNCTION_OR_METHOD))
            return make_ptr(str("<built-in function "+
                builtin_function_or_method_what(obj_cast<BUILTIN_FUNCTION_OR_METHOD>(a))+">"));
        return make_ptr(str(""));
    }
    inline POINTER __type__(const POINTER &a)
    {
        if(a->type()==typeid(_int))
            return make_ptr(TYPE::Int);
        if(a->type()==typeid(_float))
            return make_ptr(TYPE::Float);
        if(a->type()==typeid(_complex))
            return make_ptr(TYPE::Complex);
        if(a->type()==typeid(bool))
            return make_ptr(TYPE::Bool);
        if(a->type()==typeid(str))
            return make_ptr(TYPE::Str);
        if(a->type()==typeid(Tuple))
            return make_ptr(TYPE::Tuple);
        if(a->type()==typeid(python_function))
            return make_ptr(TYPE::Function);
        if(a->type()==typeid(NoneType))
            return make_ptr(TYPE::Nonetype);
        if(a->type()==typeid(Tuple_iterator))
            return make_ptr(TYPE::Tuple_iterator);
        if(a->type()==typeid(str_ascii_iterator))
            return make_ptr(TYPE::Str_ascii_iterator);
        if(a->type()==typeid(TYPE))
            return make_ptr(TYPE::Type);
        if(a->type()==typeid(BUILTIN_FUNCTION_OR_METHOD))
            return make_ptr(TYPE::Builtin_function_and_method);
        return make_ptr(TYPE::Errortype);
    }
    inline POINTER __add__(const POINTER &a,const POINTER &b)
    {
        if(a->type()==typeid(str)&&b->type()==typeid(str))
            return make_ptr(obj_cast<str>(a)+obj_cast<str>(b));
        if(a->type()==typeid(Tuple)&&b->type()==typeid(Tuple))
            return make_ptr(obj_cast<Tuple>(a)+obj_cast<Tuple>(b));
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                return make_ptr(obj_cast<_complex>(__Complex__(a))+obj_cast<_complex>(__Complex__(b)));
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))+obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))+obj_cast<_int>(__int__(b)));
        }
        throw TypeError(R"(unsupported operand type(s) for +: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    }
    inline POINTER __pos__(const POINTER &a)
    {
        if(is_type_number(a))
        {
            if(a->type()==typeid(_float)||a->type()==typeid(_complex))
                return __float__(a);
            return __int__(a);
        }
        throw TypeError(R"(bad operand type for unary +: ')"+type_name(a)+R"(')");
    }
    inline POINTER __sub__(const POINTER &a,const POINTER &b)
    {
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                return make_ptr(obj_cast<_complex>(__Complex__(a))-obj_cast<_complex>(__Complex__(b)));
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))-obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))-obj_cast<_int>(__int__(b)));
        }
        throw TypeError(R"(unsupported operand type(s) for -: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    }
    inline POINTER __neg__(const POINTER &a)
    {
        if(is_type_number(a))
        {
            if(a->type()==typeid(_complex))
                return make_ptr(-obj_cast<_complex>(a));
            if(a->type()==typeid(_float))
                return make_ptr(-obj_cast<_float>(a));
            return make_ptr(-obj_cast<_int>(__int__(a)));
        }
        throw TypeError(R"(bad operand type for unary -: ')"+type_name(a)+R"(')");
    }
    inline POINTER __mul__(const POINTER &a,const POINTER &b)
    {
        if(a->type()==typeid(str)&&(b->type()==typeid(_int)||b->type()==typeid(bool)))
            return make_ptr(any_cast<str>(a)*obj_cast<_int>(__int__(b)));
        if(b->type()==typeid(str)&&(a->type()==typeid(_int)||a->type()==typeid(bool)))
            return make_ptr(any_cast<str>(b)*obj_cast<_int>(__int__(a)));
        if(a->type()==typeid(Tuple)&&(b->type()==typeid(_int)||b->type()==typeid(bool)))
            return make_ptr(any_cast<Tuple>(a)*obj_cast<_int>(__int__(b)));
        if(b->type()==typeid(Tuple)&&(a->type()==typeid(_int)||a->type()==typeid(bool)))
            return make_ptr(any_cast<Tuple>(b)*obj_cast<_int>(__int__(a)));
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                return make_ptr(obj_cast<_complex>(__Complex__(a))*obj_cast<_complex>(__Complex__(b)));
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))*obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))*obj_cast<_int>(__int__(b)));
        }
        throw TypeError(R"(unsupported operand type(s) for *: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    }
    inline POINTER __truediv__(const POINTER &a,const POINTER &b)
    {
        if(is_type_number(a)&&is_type_number(b))
        {
            if(!obj_cast<bool>(__bool__(b)))
                throw ZeroDivisionError("division by zero");
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                return make_ptr(obj_cast<_complex>(__Complex__(a))/obj_cast<_complex>(__Complex__(b)));
            return make_ptr(obj_cast<_float>(__float__(a))/obj_cast<_float>(__float__(b)));
        }
        throw TypeError(R"(unsupported operand type(s) for /: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    }
    inline POINTER __floordiv__(const POINTER &a,const POINTER &b)
    {
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                throw TypeError(R"(unsupported operand type(s) for //: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            if(!obj_cast<bool>(__bool__(b)))
                throw ZeroDivisionError("division by zero");
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(floor(obj_cast<_float>(__float__(__truediv__(a,b)))));
            return make_ptr(obj_cast<_int>(__int__(a))/obj_cast<_int>(__int__(b)));
        }
        throw TypeError(R"(unsupported operand type(s) for //: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    }
    inline POINTER __mod__(const POINTER &a,const POINTER &b)
    {
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                throw TypeError(R"(unsupported operand type(s) for %: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
            if(!obj_cast<bool>(__bool__(b)))
                throw ZeroDivisionError("modulo by zero");
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))%obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))%obj_cast<_int>(__int__(b)));
        }
        throw TypeError(R"(unsupported operand type(s) for %: ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    }
    inline POINTER __lt__(const POINTER &a,const POINTER &b)
    {
        if(a->type()==typeid(Tuple)&&b->type()==typeid(Tuple))
            return make_ptr(obj_cast<Tuple>(a)<obj_cast<Tuple>(b));
        if(a->type()==typeid(str)&&b->type()==typeid(str))
            return make_ptr(obj_cast<str>(a)<obj_cast<str>(b));
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                throw TypeError("'<' not supported between instances of '"+type_name(a)+"' and '"+type_name(b)+"'");
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))<obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))<obj_cast<_int>(__int__(b)));
        }
        throw TypeError("'<' not supported between instances of '"+type_name(a)+"' and '"+type_name(b)+"'");
    }
    inline POINTER __pow__(const POINTER &a,const POINTER &b)
    {
        if(is_type_number(a)&&is_type_number(b))
        {
            if(!obj_cast<bool>(__bool__(a)))
            {
                if(b->type()==typeid(_complex))
                {
                    auto tmp=obj_cast<_complex>(b);
                    if(tmp.im())
                        throw ZeroDivisionError("0.0 to a negative or complex power");
                    return make_ptr(_complex(_float(0)^tmp.re()));
                }
                if(obj_cast<bool>(__lt__(b,make_ptr(_int(0)))))
                    throw ZeroDivisionError("0.0 to a negative or complex power");
            }
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                return make_ptr(obj_cast<_complex>(__Complex__(a))^obj_cast<_complex>(__Complex__(b)));
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))^obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))^obj_cast<_int>(__int__(b)));
        }
        throw TypeError(R"(unsupported operand type(s) for ** or pow(): ')"+type_name(a)+R"(' and ')"+type_name(b)+R"(')");
    }
    inline POINTER __le__(const POINTER &a,const POINTER &b)
    {
        if(a->type()==typeid(Tuple)&&b->type()==typeid(Tuple))
            return make_ptr(obj_cast<Tuple>(a)<=obj_cast<Tuple>(b));
        if(a->type()==typeid(str)&&b->type()==typeid(str))
            return make_ptr(obj_cast<str>(a)<=obj_cast<str>(b));
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                throw TypeError("'<=' not supported between instances of '"+type_name(a)+"' and '"+type_name(b)+"'");
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))<=obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))<=obj_cast<_int>(__int__(b)));
        }
        throw TypeError("'<=' not supported between instances of '"+type_name(a)+"' and '"+type_name(b)+"'");
    }
    inline POINTER __gt__(const POINTER &a,const POINTER &b)
    {
        if(a->type()==typeid(Tuple)&&b->type()==typeid(Tuple))
            return make_ptr(obj_cast<Tuple>(a)>obj_cast<Tuple>(b));
        if(a->type()==typeid(str)&&b->type()==typeid(str))
            return make_ptr(obj_cast<str>(a)>obj_cast<str>(b));
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                throw TypeError("'>' not supported between instances of '"+type_name(a)+"' and '"+type_name(b)+"'");
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))>obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))>obj_cast<_int>(__int__(b)));
        }
        throw TypeError("'>' not supported between instances of '"+type_name(a)+"' and '"+type_name(b)+"'");
    }
    inline POINTER __ge__(const POINTER &a,const POINTER &b)
    {
        if(a->type()==typeid(Tuple)&&b->type()==typeid(Tuple))
            return make_ptr(obj_cast<Tuple>(a)>=obj_cast<Tuple>(b));
        if(a->type()==typeid(str)&&b->type()==typeid(str))
            return make_ptr(obj_cast<str>(a)>=obj_cast<str>(b));
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                throw TypeError("'>=' not supported between instances of '"+type_name(a)+"' and '"+type_name(b)+"'");
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))>=obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))>=obj_cast<_int>(__int__(b)));
        }
        throw TypeError("'>=' not supported between instances of '"+type_name(a)+"' and '"+type_name(b)+"'");
    }
    inline POINTER __eq__(const POINTER &a,const POINTER &b)
    {
        if(is_type_number(a)&&is_type_number(b))
        {
            if(a->type()==typeid(_complex)||b->type()==typeid(_complex))
                return make_ptr(obj_cast<_complex>(__Complex__(a))==obj_cast<_complex>(__Complex__(b)));
            if(a->type()==typeid(_float)||b->type()==typeid(_float))
                return make_ptr(obj_cast<_float>(__float__(a))==obj_cast<_float>(__float__(b)));
            return make_ptr(obj_cast<_int>(__int__(a))==obj_cast<_int>(__int__(b)));
        }
        if(a->type()!=b->type())
            return make_ptr(false);
        if(a->type()==typeid(str))
            return make_ptr(obj_cast<str>(a)==obj_cast<str>(b));
        if(a->type()==typeid(Tuple))
            return make_ptr(obj_cast<Tuple>(a)==obj_cast<Tuple>(b));
        if(a->type()==typeid(NoneType))
            return make_ptr(true);
        return make_ptr(a==b);
    }
    inline POINTER __ne__(const POINTER &a,const POINTER &b)
    {
        return make_ptr(!obj_cast<bool>(__eq__(a,b)));
    }
    inline POINTER __logical_or__(const POINTER &a,const POINTER &b)
    {
        if(obj_cast<bool>(__bool__(a)))
            return a;
        return b;
    }
    inline POINTER __logical_and__(const POINTER &a,const POINTER &b)
    {
        if(!obj_cast<bool>(__bool__(a)))
            return a;
        return b;
    }
    inline POINTER __logical_not__(const POINTER &a)
    {
        return make_ptr(!obj_cast<bool>(__bool__(a)));
    }
    inline POINTER __getitem__(const POINTER &a,const POINTER &b)
    {
        if(b->type()!=typeid(_int)&&b->type()!=typeid(bool))
            throw TypeError("list indices must be integers or slices");
        const auto tmp=obj_cast<_int>(__int__(b));
        if(a->type()==typeid(str))
            return make_ptr(obj_cast<str>(a).__getitem__(tmp));
        if(a->type()==typeid(Tuple))
            return make_ptr(obj_cast<Tuple>(a).__getitem__(tmp));
        throw TypeError("'"+type_name(a)+R"(' object is not subscriptable)");
    }
    inline POINTER __setitem__(const POINTER &a,const POINTER &b,const POINTER &c)
    {
        // if(b->type()!=typeid(_int)&&b->type()!=typeid(bool))
        //     throw TypeError("list indices must be integers or slices");
        // const auto tmp=obj_cast<_int>(__int__(b));
        // if(a->type()==typeid(str))
        //     return ptr_cast<str>(a)->operator[]();
        // if(a->type()==typeid(Tuple))
        //     return make_ptr(obj_cast<Tuple>(a)[tmp]);
        // throw TypeError("'"+type_name(a)+R"(' object is not subscriptable)");

    }
    inline POINTER __iter__(const POINTER &a)
    {
        if(a->type()==typeid(Tuple))
            return make_ptr(Tuple_iterator(a));
        if(a->type()==typeid(Tuple_iterator))
            return a;
        if(a->type()==typeid(str))
            return make_ptr(str_ascii_iterator(a));
        if(a->type()==typeid(str_ascii_iterator))
            return a;
        throw TypeError("'"+type_name(a)+"' object is not iterable");
    }
    inline POINTER __next__(const POINTER &a)
    {
        if(a->type()==typeid(Tuple_iterator))
            return obj_cast<Tuple_iterator>(a).__next__();
        if(a->type()==typeid(str_ascii_iterator))
            return obj_cast<str_ascii_iterator>(a).__next__();
        throw TypeError("'"+type_name(a)+"' object is not an iterator");
    }
    inline POINTER __print__(const std::vector<POINTER> &a,const POINTER &sep=make_ptr(str(" ")),const POINTER &end=make_ptr(str("\n")))
    {
        for(int i=0;i<static_cast<int>(a.size());i++)
        {
            if(i)
                std::cout<<std::string(obj_cast<str>(sep));
            std::cout<<std::string(obj_cast<str>(__str__(a[i])));
        }
        std::cout<<std::string(obj_cast<str>(end));
        return make_ptr(NoneType());
    }
    inline POINTER __print__(const POINTER &a,const POINTER &sep=make_ptr(str(" ")),const POINTER &end=make_ptr(str("\n")))
    {
        std::vector<POINTER> tmp;
        tmp.emplace_back(a);
        return __print__(tmp,sep,end);
    }
    inline POINTER __tuple__(const POINTER &a=make_ptr(Tuple()))// to be written
    {
        const POINTER beg=__iter__(a);
        Tuple res;
        while(true)
        {
            try{res.val.push_back(__next__(beg));}
            catch(StopIteration&)
            {
                break;
            }
        }
        return make_ptr(res);
    }
    inline POINTER __input__(const POINTER &a=make_ptr(str("")))
    {
        __print__(a,make_ptr(str(" ")),make_ptr(str("")));
        std::string s;
        getline(std::cin,s);
        return make_ptr(str(s));
    }
    inline POINTER __min__(const std::vector<POINTER> &a)
    {
        if(a.size()==1)
        {
            const POINTER beg=__iter__(a[0]);
            POINTER res;
            try
            {
                res=__next__(beg);
            }
            catch(StopIteration&)
            {
                throw ValueError("min() iterable argument is empty");
            }
            while(true)
            {
                try
                {
                    if(const POINTER now=__next__(beg); obj_cast<bool>(__lt__(now,res)))
                        res=now;
                }
                catch(StopIteration&)
                {
                    break;
                }
            }
            return res;
        }
        POINTER res=a[0];
        for(int i=1;i<static_cast<int>(a.size());i++)
            if(obj_cast<bool>(__lt__(a[i],res)))
                res=a[i];
        return res;
    }
    inline POINTER __max__(const std::vector<POINTER> &a)
    {
        if(a.size()==1)
        {
            const POINTER beg=__iter__(a[0]);
            POINTER res;
            try
            {
                res=__next__(beg);
            }
            catch(StopIteration&)
            {
                throw ValueError("max() iterable argument is empty");
            }
            while(true)
            {
                try
                {
                    if(const POINTER& now=__next__(beg); obj_cast<bool>(__gt__(now,res)))
                        res=now;
                }
                catch(StopIteration&)
                {
                    break;
                }
            }
            return res;
        }
        POINTER res=a[0];
        for(int i=1;i<static_cast<int>(a.size());i++)
            if(obj_cast<bool>(__gt__(a[i],res)))
                res=a[i];
        return res;
    }
    inline POINTER __abs__(const POINTER &a)
    {
        if(is_type_number(a))
        {
            if(a->type()==typeid(_complex))
                return make_ptr(abs(obj_cast<_complex>(a)));
            if(std::any_cast<bool>(__ge__(a,make_ptr(_int(0)))))
                return __pos__(a);
            return __neg__(a);
        }
        throw TypeError("bad operand type for abs()");
    }
    inline POINTER __sum__(const POINTER &a,POINTER sum=make_ptr(_int(0)))
    {
        if(sum->type()==typeid(str))
            throw TypeError("sum() can't sum strings");
        const POINTER beg=__iter__(a);
        while(true)
        {
            try
            {
                sum=__add__(sum,__next__(beg));
            }
            catch(StopIteration&)
            {
                break;
            }
        }
        return sum;
    }
    inline POINTER __eval__(const POINTER &a)
    {
        if(a->type()!=typeid(str))
            throw TypeError("eval() arg 1 must be a string");
        const auto LS_=LS,RS_=RS,FA_=FA;
        const auto RAW_=raw_expression_;
        LS.clear(),RS.clear(),FA.clear(),raw_expression_.clear();
        std::vector<std::pair<int,std::vector<std::any>>> vec;
        translator(static_cast<std::string>(obj_cast<str>(a)),true);
        processor();
        const POINTER res=executor_single(builder(),false);
        LS=LS_,RS=RS_,FA=FA_,raw_expression_=RAW_;
        return res;
    }
}

inline std::strong_ordering operator<=>(const Tuple &a, const Tuple &b)
{
    const int siz=std::min(a.SIZE(),b.SIZE());
    for(int i=0;i<siz;i++)
    {
        if(obj_cast<bool>(inner_scope::__lt__(a.__getitem__(i),b.__getitem__(i))))
            return std::strong_ordering::less;
        if(obj_cast<bool>(inner_scope::__gt__(a.__getitem__(i),b.__getitem__(i))))
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
        if(obj_cast<bool>(inner_scope::__ne__(a.__getitem__(i),b.__getitem__(i))))
            return false;
    return true;
}

/*
 *This part of code is for translator
 */
//some tools for translator
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
        return NoneType();
    if(!con.empty()&&(con.front()=='\''||con.front()=='\"'))
        return str(con.substr(1,con.size()-2));
    if(con.back()=='j')
    {
        _float tmp;
        try
        {
            tmp=_float(con.substr(0,con.size()-1));
        }
        catch(...){throw SyntaxError("invalid decimal literal");}
        return _complex(_float(0),tmp);
    }
    const int start=(con.front()=='+'||con.front()=='-');
    if(start==con.size())
        throw SyntaxError("invalid syntax");
    int base=10;
    if(con[start]=='0')
    {
        if(con.size()==start+1)
            return _int(0);
        if(con[start+1]=='b'||con[start+1]=='B')
            base=2;
        if(con[start+1]=='o'||con[start+1]=='O')
            base=8;
        if(con[start+1]=='x'||con[start+1]=='X')
            base=16;
    }
    if(base==10)
    {
        try
        {
            const auto& tmp=_int(con);
            if(!tmp&&con[start]=='0')
                throw SyntaxError("leading zeros in decimal integer literals are not permitted; use an 0o prefix for octal integers");
            return tmp;
        }
        catch(...){}
        try{return _float(con);}
        catch(...){throw SyntaxError("invalid decimal literal");}
    }
    if(base==2)
    {
        try{return *inner_scope::__int__(make_ptr(str(con)),make_ptr(_int(2)),true);}
        catch(...){throw SyntaxError("invalid binary literal");}
    }
    if(base==8)
    {
        try{return *inner_scope::__int__(make_ptr(str(con)),make_ptr(_int(8)),true);}
        catch(...){throw SyntaxError("invalid octal literal");}
    }
    if(base==16)
    {
        try{return *inner_scope::__int__(make_ptr(str(con)),make_ptr(_int(16)),true);}
        catch(...){throw SyntaxError("invalid hexadecimal literal");}
    }
    throw SyntaxError("Unknown error");
}
inline std::any translator_any(const std::string &s)
{
    if(const auto tmp=translator_operator(s);tmp!=OPERATOR::ILLEGAL)
        return tmp;
    if(const auto tmp=translator_keyword(s);tmp!=KEYWORD::ILLEGAL)
        return tmp;
    if((s[0]>='0'&&s[0]<='9')||s[0]=='+'||s[0]=='-'||s[0]=='.')
        return translator_literal(s);
    try{return translator_literal(s);}
    catch(...)
    {
        if(!is_variable(s))
            throw SyntaxError("unknown object "+s);
        return s;
    }

}

inline int mode=0,IN_EOF=0,INDENTATION_COUNT;

/*
 *translate s into machine-identifiable language (stored in raw_expression_) and return the state of translation.
 *If normally ends,return 1,otherwise return 0.
 */
inline int translator(const std::string &s,const bool option)
{
    static std::string las;
    static int state=-1,shift=0,decimal_state=1;
    static std::vector<std::string> pre;
    static std::vector<std::string> sta;
    IN_EOF=0;
    if(!mode)
    {
        mode=1,INDENTATION_COUNT=get_indentation(s);
        state=-1,shift=0,decimal_state=1;
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
                    throw SyntaxError("invalid syntax");
                IN_EOF=1;
                if(las=="("||las=="["||las=="{")
                    sta.push_back(las);
                if(las==")"||las=="]"||las=="}")
                {
                    if(sta.empty())
                        throw SyntaxError("unmatched \'"+las+"\'");
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
                if(is_legal(t))
                {
                    state=0,las=t;
                    decimal_state=is_number(t);
                }
                else if(t=='.')
                    state=1,las=t;
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
                    las.clear(),state=-1,raw_expression_.emplace_back(ErrorType());
            }
            /*
             *non-string literal mode
             *maybe operator,keyword,literal,variable
             *we have to identify its type
             */
            else if(state==0)
            {
                if(is_legal(t))
                {
                    state=0,las+=t;
                    decimal_state&=is_number(t);
                }
                else if(t=='.')
                {
                    if(!decimal_state)
                        state=1,raw_expression_.push_back(translator_any(las)),las=t;
                    else
                        las+=t;
                }
                else if(is_operator(t))
                {
                    if((t=='+'||t=='-')&&las.back()=='e')
                    {
                        int ck=1;
                        try{_float(las.substr(0,las.size()-1));}
                        catch(...){ck=0;}
                        if(ck)
                        {
                            state=0,las+=t;
                            continue;
                        }
                    }
                    state=1,raw_expression_.push_back(translator_any(las)),las=t;
                }
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
                    las.clear(),state=-1,raw_expression_.emplace_back(ErrorType());
            }
            else if(state==1)// operator mode
            {
                if(las=="("||las=="["||las=="{")
                    sta.push_back(las);
                if(las==")"||las=="]"||las=="}")
                {
                    if(sta.empty())
                        throw SyntaxError("unmatched \'"+las+"\'");
                    sta.pop_back();
                }
                if(las=="}")
                {
                    if(pre.empty())
                        throw SyntaxError("unmatched \'}\'");
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
                if(las=="."&&is_number(t))
                {
                    state=0,las+=t;
                    continue;
                }
                if(is_legal(t))
                    state=0,raw_expression_.emplace_back(translator_operator(las)),las=t;
                else if(is_operator(t))
                {
                    if((t=='='&&(las=="="||las=="<"||las==">"||las=="!"||las=="+"||las=="-"||las=="*"||las=="/"||
                        las=="//"||las=="%"||las=="**"||las==":"))||(las=="*"&&t=='*')||(las=="/"&&t=='/'))
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
                    las.clear(),state=-1,raw_expression_.emplace_back(ErrorType());
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
                        state=-1,sta.emplace_back("{");
                    }
                }
                else if(t=='}')
                {
                    if(i+1==s.size()||s[i+1]!='}')
                        throw SyntaxError(R"(f-string: single '}' is not allowed)");
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
                        throw SyntaxError(R"(f-string: single '}' is not allowed)");
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
                throw SyntaxError("unmatched \'"+las+"\'");
            sta.pop_back();
        }
        if(las=="}")
        {
            if(pre.empty())
                throw SyntaxError("unmatched \'}\'");
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
            throw SyntaxError("\'"+sta.back()+R"(' was never closed)");
        }
        if(state==2||state==3)
            throw SyntaxError("unterminated string literal");
        if(state==4||state==5)
            throw SyntaxError("unterminated f-string literal");
        return mode=0,1;
    }
    catch(SyntaxError &a)
    {
        mode=0,raw_expression_.clear();
        throw SyntaxError(a.what());
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
        if(t.type()==typeid(ErrorType))
            throw SyntaxError("illegal symbol");
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
                    throw SyntaxError("invalid syntax");
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
                    throw SyntaxError("invalid syntax");
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
                    throw SyntaxError("invalid syntax");
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
                    throw SyntaxError("invalid syntax");
                if(i!=l)
                {
                    if(raw_expression_[i-1].type()!=typeid(OPERATOR))
                        throw SyntaxError("invalid syntax");
                    if(const auto tmp_=std::any_cast<OPERATOR>(raw_expression_[i-1]);
                        tmp_!=OPERATOR::LPAR&&tmp_!=OPERATOR::LBRA&&tmp_!=OPERATOR::OR&&tmp_!=OPERATOR::AND&&
                        tmp_!=OPERATOR::NOT&&tmp_!=OPERATOR::ASSIGN&&tmp_!=OPERATOR::ADDASS&&tmp_!=OPERATOR::SUBASS&&
                        tmp_!=OPERATOR::MULASS&&tmp_!=OPERATOR::DIVASS&&tmp_!=OPERATOR::MODASS&&
                        tmp!=OPERATOR::IDIVASS&&tmp!=OPERATOR::POWASS&&tmp!=OPERATOR::REAASS&&tmp_!=OPERATOR::CALL)
                        throw SyntaxError("invalid syntax");
                }
                FA[i+1]=i,LS[i]=i+1;
            }
            else if(tmp==OPERATOR::LPAR||tmp==OPERATOR::PLUS|tmp==OPERATOR::MINUS)
            {
                if(i+1==r)
                    throw SyntaxError(R"('(' was never closed)");
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
                        throw SyntaxError(R"(closing parenthesis ')' does not match opening parenthesis '[')");
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
                    throw SyntaxError(R"(unmatched ')')");
                if(std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LBRA)
                    throw SyntaxError(R"(closing parenthesis ')' does not match opening parenthesis '[')");
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
                    throw SyntaxError("invalid syntax");
                if(pos>=l&&raw_expression_[pos].type()==typeid(OPERATOR)&&
                    (std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LPAR||
                    std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::CALL))
                    throw SyntaxError(R"(closing parenthesis ']' does not match opening parenthesis '(')");
                while(pos>=l&&(raw_expression_[pos].type()!=typeid(OPERATOR)||
                    (std::any_cast<OPERATOR>(raw_expression_[pos])!=OPERATOR::LPAR&&
                    std::any_cast<OPERATOR>(raw_expression_[pos])!=OPERATOR::LBRA&&
                    std::any_cast<OPERATOR>(raw_expression_[pos])!=OPERATOR::CALL)))
                    pos=FA[pos];
                if(pos==-1)
                    throw SyntaxError(R"(unmatched ']')");
                if(std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::LPAR||
                    std::any_cast<OPERATOR>(raw_expression_[pos])==OPERATOR::CALL)
                    throw SyntaxError(R"(closing parenthesis ']' does not match opening parenthesis '(')");
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
                    throw SyntaxError("invalid syntax");
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
            throw SyntaxError("invalid syntax");
    }
    int now=r-1;
    while(now!=-1)
    {
        if(raw_expression_[now].type()==typeid(OPERATOR)&&
            (std::any_cast<OPERATOR>(raw_expression_[now])==OPERATOR::LPAR||
            std::any_cast<OPERATOR>(raw_expression_[now])==OPERATOR::CALL))
            throw SyntaxError(R"('(' was never closed)");
        if(raw_expression_[now].type()==typeid(OPERATOR)&&
            std::any_cast<OPERATOR>(raw_expression_[now])==OPERATOR::LBRA)
            throw SyntaxError(R"('[' was never closed)");
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
                throw SyntaxError("invalid syntax");
            return 0;
        }
        else
        {
            if(fi==KEYWORD::GLOBAL||fi==KEYWORD::NONLOCAL)
            {
                if(static_cast<int>(raw_expression_.size())==1)
                    throw SyntaxError("invalid syntax");
                return LS[0]=builder_arithmetic(1),FA[LS[0]]=0,0;
            }
            if(fi==KEYWORD::IF||fi==KEYWORD::ELIF||fi==KEYWORD::WHILE||fi==KEYWORD::DEF)
            {
                const int pos=find_colon();
                if(pos==-1)
                    throw SyntaxError(R"(expected ':')");
                if(pos==1)
                    throw SyntaxError("invalid syntax");
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
                                throw SyntaxError("invalid syntax");
                            LS[pos+1]=builder_arithmetic(pos+2),FA[LS[pos+1]]=pos+1;
                        }
                        else if(tmp==KEYWORD::BREAK||tmp==KEYWORD::CONTINUE||tmp==KEYWORD::PASS)
                        {
                            if(pos+2!=static_cast<int>(raw_expression_.size()))
                                throw SyntaxError("invalid syntax");
                        }
                        else if(tmp==KEYWORD::RETURN)
                        {
                            if(static_cast<int>(raw_expression_.size())!=pos+2)
                                LS[pos+1]=builder_arithmetic(pos+2),FA[LS[pos+1]]=pos+1;
                        }
                        else
                            throw SyntaxError("invalid syntax");
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
                    throw SyntaxError(R"(expected ':')");
                if(static_cast<int>(raw_expression_.size())!=2)
                {
                    if(raw_expression_[2].type()==typeid(KEYWORD))
                    {
                        const auto tmp=std::any_cast<KEYWORD>(raw_expression_[2]);
                        RS[0]=2,FA[RS[0]]=0;
                        if(tmp==KEYWORD::GLOBAL||tmp==KEYWORD::NONLOCAL)
                        {
                            if(static_cast<int>(raw_expression_.size())==3)
                                throw SyntaxError("invalid syntax");
                            LS[2]=builder_arithmetic(3),FA[LS[2]]=2;
                        }
                        else if(tmp==KEYWORD::BREAK||tmp==KEYWORD::CONTINUE||tmp==KEYWORD::PASS)
                        {
                            if(static_cast<int>(raw_expression_.size())!=3)
                                throw SyntaxError("invalid syntax");
                        }
                        else if(tmp==KEYWORD::RETURN)
                        {
                            if(static_cast<int>(raw_expression_.size())!=3)
                                LS[2]=builder_arithmetic(3),FA[LS[2]]=2;
                        }
                        else
                            throw SyntaxError("invalid syntax");
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
inline POINTER executor_block(const std::vector<std::pair<int,std::vector<std::any>>>&,int,
    const std::vector<DICT_PTR>&);
inline void make_binding(const std::string &s,const DICT_PTR &d)
{
    if(!d->contains(s))
        (*d)[s]=nullptr;
    MY_DICT[s]=d;
}
inline void find_binding(const std::string &s)
{
    if(!BIND_DICT.contains(s))
        BIND_DICT[s]=GLOBAL_STATE?0:2;
    if(const int x=BIND_DICT[s]; !x)
        make_binding(s,LOCAL_DICT);
    else if(x==1)
        make_binding(s,NONLOCAL_DICT);
    else
        make_binding(s,GLOBAL_DICT);
}
inline POINTER executor_arithmetic(const int &rt)
{
    std::vector<int> stack;
    stack.push_back(rt);
    while(!stack.empty())
    {
        if(const int x=stack.back(); raw_expression_[x].type()==typeid(KEYWORD))
            throw SyntaxError("invalid syntax");
        else if(LS[x]<0&&RS[x]<0)
        {
            stack.pop_back();
            if(raw_expression_[x].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[x])==OPERATOR::RPAR)
                RES[x]=make_ptr(Tuple());
            else
                RES[x]=get_ptr(raw_expression_[x]);
        }
        else if(LS[x]>=0&&FA[LS[x]]==x)
            stack.push_back(LS[x]),FA[LS[x]]=-1;
        else if(raw_expression_[x].type()==typeid(OPERATOR)&&
            std::any_cast<OPERATOR>(raw_expression_[x])==OPERATOR::CALL_)
        {
            stack.pop_back();
            if(RES[LS[x]]->type()==typeid(TYPE))
            {
                const auto now_func=obj_cast<TYPE>(RES[LS[x]]);
                std::vector<int> sta;
                std::vector<POINTER> par;
                sta.push_back(RS[x]);
                while(!sta.empty())
                {
                    const int now=sta.back();
                    sta.pop_back();
                    if(now==-1)
                        continue;
                    if(raw_expression_[now].type()==typeid(OPERATOR))
                    {
                        if(const auto tmp=std::any_cast<OPERATOR>(raw_expression_[now]); tmp==OPERATOR::COMMA)
                        {
                            sta.push_back(RS[now]);
                            sta.push_back(LS[now]);
                            continue;
                        }
                        else if(tmp==OPERATOR::ALLO)
                            throw TypeError("invalid keyword argument");
                    }
                    par.push_back(executor_single(now,false));
                }
                if(now_func==TYPE::Int)
                {
                    if(par.empty())
                        RES[x]=inner_scope::__int__();
                    else if(par.size()==1)
                        RES[x]=inner_scope::__int__(par[0]);
                    else if(par.size()==2)
                        RES[x]=inner_scope::__int__(par[0],par[1],true);
                    else
                        throw TypeError("int() takes at most 2 arguments");
                }
                else if(now_func==TYPE::Float)
                {
                    if(par.empty())
                        RES[x]=inner_scope::__float__();
                    else if(par.size()==1)
                        RES[x]=inner_scope::__float__(par[0]);
                    else
                        throw TypeError("float expected at most 1 argument");
                }
                else if(now_func==TYPE::Bool)
                {
                    if(par.empty())
                        RES[x]=inner_scope::__bool__();
                    else if(par.size()==1)
                        RES[x]=inner_scope::__bool__(par[0]);
                    else
                        throw TypeError("bool expected at most 1 argument");
                }
                else if(now_func==TYPE::Str)
                {
                    if(par.empty())
                        RES[x]=inner_scope::__str__();
                    else if(par.size()==1)
                        RES[x]=inner_scope::__str__(par[0]);
                    else
                        throw TypeError("str() takes at most 1 argument");
                }
                else if(now_func==TYPE::Tuple)
                {
                    if(par.empty())
                        RES[x]=inner_scope::__tuple__();
                    else if(par.size()==1)
                        RES[x]=inner_scope::__tuple__(par[0]);
                    else
                        throw TypeError("tuple expected at most 1 argument");
                }
                else if(now_func==TYPE::Complex)
                {
                    if(par.empty())
                        RES[x]=inner_scope::__Complex__();
                    else if(par.size()==1)
                        RES[x]=inner_scope::__Complex__(par[0]);
                    else if(par.size()==2)
                        RES[x]=inner_scope::__Complex__(par[0],par[1]);
                    else
                        throw TypeError("complex() takes at most 2 arguments");
                }
                else if(now_func==TYPE::Type)
                {
                    if(par.size()!=1)
                        throw TypeError("type() takes exactly one argument");
                    RES[x]=inner_scope::__type__(par[0]);
                }
            }
            else if(RES[LS[x]]->type()==typeid(BUILTIN_FUNCTION_OR_METHOD))
            {
                const auto now_func=obj_cast<BUILTIN_FUNCTION_OR_METHOD>(RES[LS[x]]);
                std::vector<int> sta;
                std::vector<POINTER> par;
                sta.push_back(RS[x]);
                while(!sta.empty())
                {
                    const int now=sta.back();
                    sta.pop_back();
                    if(now==-1)
                        continue;
                    if(raw_expression_[now].type()==typeid(OPERATOR))
                    {
                        if(const auto tmp=std::any_cast<OPERATOR>(raw_expression_[now]); tmp==OPERATOR::COMMA)
                        {
                            sta.push_back(RS[now]);
                            sta.push_back(LS[now]);
                            continue;
                        }
                        else if(tmp==OPERATOR::ALLO)
                            throw TypeError("invalid keyword argument");
                    }
                    par.push_back(executor_single(now,false));
                }
                if(now_func==BUILTIN_FUNCTION_OR_METHOD::PRINT)
                    RES[x]=inner_scope::__print__(par);
                else if(now_func==BUILTIN_FUNCTION_OR_METHOD::INPUT)
                {
                    if(par.empty())
                        RES[x]=inner_scope::__input__();
                    else if(par.size()==1)
                        RES[x]=inner_scope::__input__(par[0]);
                    if(par.size()>1)
                        throw TypeError("input expected at most 1 argument");
                }
                else if(now_func==BUILTIN_FUNCTION_OR_METHOD::MIN)
                {
                    if(par.empty())
                        throw TypeError("min expected at least 1 argument");
                    RES[x]=inner_scope::__min__(par);
                }
                else if(now_func==BUILTIN_FUNCTION_OR_METHOD::MAX)
                {
                    if(par.empty())
                        throw TypeError("max expected at least 1 argument");
                    RES[x]=inner_scope::__max__(par);
                }
                else if(now_func==BUILTIN_FUNCTION_OR_METHOD::ABS)
                {
                    if(par.size()!=1)
                        throw TypeError("abs() takes exactly one argument");
                    RES[x]=inner_scope::__abs__(par[0]);
                }
                else if(now_func==BUILTIN_FUNCTION_OR_METHOD::SUM)
                {
                    if(par.empty())
                        throw TypeError("sum() takes at least 1 argument");
                    if(par.size()>=3)
                        throw TypeError("sum() takes at most 2 arguments");
                    if(par.size()==1)
                        RES[x]=inner_scope::__sum__(par[0]);
                    else
                        RES[x]=inner_scope::__sum__(par[0],par[1]);
                }
                else if(now_func==BUILTIN_FUNCTION_OR_METHOD::EVAL)
                {
                    if(par.size()!=1)
                        throw TypeError("eval takes exactly one argument");
                    RES[x]=inner_scope::__eval__(par[0]);
                }
                else if(now_func==BUILTIN_FUNCTION_OR_METHOD::ITER)
                {
                    if(par.size()!=1)
                        throw TypeError("iter takes exactly one argument");
                    RES[x]=inner_scope::__iter__(par[0]);
                }
                else if(now_func==BUILTIN_FUNCTION_OR_METHOD::NEXT)
                {
                    if(par.size()!=1)
                        throw TypeError("next takes exactly one argument");
                    RES[x]=inner_scope::__next__(par[0]);
                }
            }
            else if(RES[LS[x]]->type()==typeid(python_function))
            {
                auto now_func=obj_cast<python_function>(RES[LS[x]]);
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
                const auto RAW=raw_expression_;
                const auto RES_=RES;
                LS.clear(),RS.clear(),FA.clear(),raw_expression_.clear(),RES.clear();
                POINTER tmp_res;
                std::exception_ptr exc=nullptr;
                try{tmp_res=executor_block(now_func.code,true,now_func.PRE_DICT);}
                catch(...){exc=std::current_exception();}
                LS=LS_,RS=RS_,FA=FA_,raw_expression_=RAW,RES=RES_;
                RES[x]=tmp_res;
                GLOBAL_STATE=PRE_GLOBAL_STATE;
                LOCAL_DICT=PRE_LOCAL_DICT;
                NONLOCAL_DICT=PRE_NONLOCAL_DICT;
                MY_DICT=PRE_MY_DICT;
                BIND_DICT=PRE_BIND_DICT;
                std::rethrow_exception(exc);
            }
            else
                throw TypeError("\'"+type_name(RES[LS[x]])+"\' object is not callable");
        }
        else if(RS[x]>=0&&FA[RS[x]]==x)
        {
            if(const auto tmp=std::any_cast<OPERATOR>(raw_expression_[x]);
                tmp==OPERATOR::OR&&obj_cast<bool>(inner_scope::__bool__(RES[LS[x]]))||
                (tmp==OPERATOR::AND&&!obj_cast<bool>(inner_scope::__bool__(RES[LS[x]]))))
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
                RES[x]=inner_scope::__add__(RES[LS[x]],RES[RS[x]]);
            else if(tmp==OPERATOR::SUB)
                RES[x]=inner_scope::__sub__(RES[LS[x]],RES[RS[x]]);
            else if(tmp==OPERATOR::MUL)
                RES[x]=inner_scope::__mul__(RES[LS[x]],RES[RS[x]]);
            else if(tmp==OPERATOR::DIV)
                RES[x]=inner_scope::__truediv__(RES[LS[x]],RES[RS[x]]);
            else if(tmp==OPERATOR::MOD)
                RES[x]=inner_scope::__mod__(RES[LS[x]],RES[RS[x]]);
            else if(tmp==OPERATOR::IDIV)
                RES[x]=inner_scope::__floordiv__(RES[LS[x]],RES[RS[x]]);
            else if(tmp==OPERATOR::POW)
                RES[x]=inner_scope::__pow__(RES[LS[x]],RES[RS[x]]);
            else if(tmp==OPERATOR::OR||tmp==OPERATOR::AND)
                RES[x]=RES[RS[x]];
            else if(tmp==OPERATOR::NOT)
                RES[x]=inner_scope::__logical_not__(RES[LS[x]]);
            else if(tmp==OPERATOR::PLUS)
                RES[x]=inner_scope::__pos__(RES[LS[x]]);
            else if(tmp==OPERATOR::MINUS)
                RES[x]=inner_scope::__neg__(RES[LS[x]]);
            else if(tmp==OPERATOR::RPAR)
                RES[x]=RES[LS[x]];
            else if(tmp==OPERATOR::RBRA)
                RES[x]=inner_scope::__getitem__(RES[LS[x]],RES[RS[x]]);
            else if(tmp==OPERATOR::LE||tmp==OPERATOR::LEQ||tmp==OPERATOR::GE||tmp==OPERATOR::GEQ||tmp==OPERATOR::EQ||
                tmp==OPERATOR::NEQ||tmp==OPERATOR::IS||tmp==OPERATOR::IN||tmp==OPERATOR::IS_NOT||tmp==OPERATOR::NOT_IN)
            {
                auto fi=RES[LS[x]];
                if(raw_expression_[LS[x]].type()==typeid(OPERATOR))
                {
                    if(const auto pre=std::any_cast<OPERATOR>(raw_expression_[LS[x]]);
                        pre==OPERATOR::LE||pre==OPERATOR::LEQ||pre==OPERATOR::GE||pre==OPERATOR::GEQ||pre==OPERATOR::EQ||
                        pre==OPERATOR::NEQ||pre==OPERATOR::IS||pre==OPERATOR::IN||pre==OPERATOR::IS_NOT||
                        pre==OPERATOR::NOT_IN)
                        fi=RES[RS[LS[x]]];
                }
                if(tmp==OPERATOR::LE)
                    RES[x]=inner_scope::__lt__(fi,RES[RS[x]]);
                if(tmp==OPERATOR::LEQ)
                    RES[x]=inner_scope::__le__(fi,RES[RS[x]]);
                if(tmp==OPERATOR::GE)
                    RES[x]=inner_scope::__gt__(fi,RES[RS[x]]);
                if(tmp==OPERATOR::GEQ)
                    RES[x]=inner_scope::__ge__(fi,RES[RS[x]]);
                if(tmp==OPERATOR::EQ)
                    RES[x]=inner_scope::__eq__(fi,RES[RS[x]]);
                if(tmp==OPERATOR::NEQ)
                    RES[x]=inner_scope::__ne__(fi,RES[RS[x]]);
            }
            else if(tmp==OPERATOR::COMMA)
            {
                Tuple lR;
                if(raw_expression_[LS[x]].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[LS[x]])==OPERATOR::COMMA)
                    lR=obj_cast<Tuple>(RES[LS[x]]);
                else
                    lR=Tuple(RES[LS[x]]);
                if(RS[x]==-1)
                    RES[x]=make_ptr(lR);
                else
                {
                    Tuple rR;
                    if(raw_expression_[RS[x]].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[RS[x]])==OPERATOR::COMMA)
                        rR=std::any_cast<Tuple>(RES[RS[x]]);
                    else
                        rR=Tuple(RES[RS[x]]);
                    RES[x]=make_ptr(lR+rR);
                }
            }
            else
                throw SyntaxError("invalid syntax");
        }
    }
    return RES[rt];
}

inline void assign(const std::string &s,const POINTER &a)
{
    find_binding(s);
    (*MY_DICT[s])[s]=a;
}
inline void pre_packer(const int x)
{
    if(x==-1)
        return ;
    if(raw_expression_[x].type()==typeid(OPERATOR))
    {
        const auto now=std::any_cast<OPERATOR>(raw_expression_[x]);
        if(now==OPERATOR::COMMA)
        {
            pre_packer(LS[x]);
            pre_packer(RS[x]);
            return ;
        }
        if(now==OPERATOR::RPAR)
            return pre_packer(LS[x]);
        if(now==OPERATOR::RBRA)
            return ;
        throw SyntaxError("cannot assign to expression");
    }
    if(raw_expression_[x].type()!=typeid(std::string))
        throw SyntaxError("cannot assign to literal");
    find_binding(std::any_cast<std::string>(raw_expression_[x]));
}
inline void unpacker(int a,const POINTER &right)
{
    std::vector<int> left;
    int ck=0;
    while(a!=-1&&raw_expression_[a].type()==typeid(OPERATOR)&&
        std::any_cast<OPERATOR>(raw_expression_[a])==OPERATOR::COMMA)
    {
        ck=1;
        if(RS[a]!=-1)
            left.push_back(RS[a]);
        a=LS[a];
    }
    if(left.empty()&&a!=-1&&!ck)
    {
        if(raw_expression_[a].type()==typeid(OPERATOR))
        {
            if(std::any_cast<OPERATOR>(raw_expression_[a])==OPERATOR::RPAR)
                return unpacker(LS[a],right);
            throw SyntaxError("cannot assign to expression here.");
        }
        if(raw_expression_[a].type()!=typeid(std::string))
            throw SyntaxError("cannot assign to literal here.");
        assign(std::any_cast<std::string>(raw_expression_[a]),right);
        return ;
    }
    if(a!=-1)
        left.push_back(a);
    std::vector<POINTER> res;
    const auto iter=inner_scope::__iter__(right);
    while(true)
    {
        try{res.push_back(inner_scope::__next__(iter));}
        catch(...){break;}
    }
    if(res.size()<left.size())
        throw ValueError("not enough values to unpack (expected "+std::to_string(left.size())+", got "
            +std::to_string(res.size())+")");
    if(res.size()>left.size())
        throw ValueError("too many values to unpack (expected "+std::to_string(left.size())+")");
    std::ranges::reverse(left);
    for(int i=0;i<res.size();i++)
        unpacker(left[i],res[i]);
}
inline void executor_assignment(const int &rt)
{
    if(LS[rt]==-1||RS[rt]==-1)
        throw SyntaxError("invalid syntax");
    if(const auto ope=std::any_cast<OPERATOR>(raw_expression_[rt]); ope==OPERATOR::ASSIGN)
    {
        std::vector<int> sta,fd;
        sta.push_back(LS[rt]);
        while(!sta.empty())
        {
            const int x=sta.back();sta.pop_back();
            if(raw_expression_[x].type()==typeid(OPERATOR)&&
                std::any_cast<OPERATOR>(raw_expression_[x])==OPERATOR::ASSIGN)
                sta.push_back(LS[x]),sta.push_back(RS[x]);
            else
                fd.push_back(x),pre_packer(x);
        }
        const auto tmp=executor_arithmetic(RS[rt]);
        for(const auto x:fd)
            unpacker(x,tmp);
    }
    else
    {
        int now=LS[rt];
        while(now!=-1&&raw_expression_[now].type()==typeid(OPERATOR)&&std::any_cast<OPERATOR>(raw_expression_[now])==OPERATOR::RPAR)
            now=LS[rt];
        if(now==-1)
            throw SyntaxError(R"('tuple' is an illegal expression for augmented assignment)");
        if(raw_expression_[now].type()==typeid(OPERATOR))
        {
            const auto tmp=std::any_cast<OPERATOR>(raw_expression_[now]);
            if(tmp==OPERATOR::COMMA)
                throw SyntaxError(R"('tuple' is an illegal expression for augmented assignment)");
            // setitem to be written
            else
                throw SyntaxError("'expression' is an illegal expression for augmented assignment");
        }
        if(raw_expression_[now].type()!=typeid(std::string))
            throw SyntaxError(R"('literal' is an illegal expression for augmented assignment)");
        const auto tmp=executor_arithmetic(RS[rt]);
        const auto lf=raw_expression_[now];
        find_binding(std::any_cast<std::string>(lf));
        if(ope==OPERATOR::ADDASS)
            assign(std::any_cast<std::string>(lf),inner_scope::__add__(get_ptr(lf),tmp));
        if(ope==OPERATOR::SUBASS)
            assign(std::any_cast<std::string>(lf),inner_scope::__sub__(get_ptr(lf),tmp));
        if(ope==OPERATOR::MULASS)
            assign(std::any_cast<std::string>(lf),inner_scope::__mul__(get_ptr(lf),tmp));
        if(ope==OPERATOR::DIVASS)
            assign(std::any_cast<std::string>(lf),inner_scope::__truediv__(get_ptr(lf),tmp));
        if(ope==OPERATOR::MODASS)
            assign(std::any_cast<std::string>(lf),inner_scope::__mod__(get_ptr(lf),tmp));
        if(ope==OPERATOR::IDIVASS)
            assign(std::any_cast<std::string>(lf),inner_scope::__floordiv__(get_ptr(lf),tmp));
        if(ope==OPERATOR::POWASS)
            assign(std::any_cast<std::string>(lf),inner_scope::__pow__(get_ptr(lf),tmp));
    }
}
inline POINTER executor_single(const int &rt,const bool type)
{
    RES.resize(static_cast<int>(raw_expression_.size()));
    if(raw_expression_[rt].type()==typeid(OPERATOR)&&type)
    {
        const auto ope=std::any_cast<OPERATOR>(raw_expression_[rt]);
        if(ope==OPERATOR::ASSIGN||ope==OPERATOR::ADDASS||ope==OPERATOR::SUBASS||ope==OPERATOR::MULASS||
            ope==OPERATOR::DIVASS||ope==OPERATOR::MODASS||ope==OPERATOR::IDIVASS||ope==OPERATOR::POWASS)
            return executor_assignment(rt),make_ptr(NoneType());
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
        (*LOCAL_DICT)[fi]=se;
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
                    throw SyntaxError("invalid syntax");
                const auto name=std::any_cast<std::string>(raw_expression_[LS[x]]);
                if(!is_variable(name))
                    throw SyntaxError("invalid syntax");
                const auto val=executor_arithmetic(RS[x]);
                if(!para_dict.contains(name))
                    throw TypeError("unexpected keyword argument \'"+name+"\'");
                if(chk.contains(name))
                    throw SyntaxError("multiple values for argument \'"+name+"\'");
                chk[name]=true;
                (*LOCAL_DICT)[name]=val;
                continue;
            }
        }
        if(op)
            throw SyntaxError("positional argument follows keyword argument");
        if(cnt==parameter.size())
            throw TypeError("too much positional arguments");
        const auto name=parameter[cnt++];
        const auto val=executor_single(x,false);
        if(!para_dict.contains(name))
            throw TypeError("unexpected keyword argument \'"+name+"\'");
        if(chk.contains(name))
            throw TypeError("multiple values for argument \'"+name+"\'");
        chk[name]=true;
        (*LOCAL_DICT)[name]=val;
    }
    for(const auto &t:parameter)
        if((*LOCAL_DICT)[t]->type()==typeid(std::string))
            throw TypeError("missing argument \'"+t+"\'");
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
                    throw SyntaxError("invalid syntax");
                const auto name=std::any_cast<std::string>(raw_expression_[LS[x]]);
                if(!is_variable(name))
                    throw SyntaxError("invalid syntax");
                const auto val=executor_single(RS[x],false);
                parameter.push_back(name);
                if(para_dict.contains(name))
                    throw SyntaxError("duplicate argument \'"+name+"\' in function definition");
                para_dict[name]=val;
                continue;
            }
        }
        if(op)
            throw SyntaxError("parameter without a default follows parameter with a default");
        if(raw_expression_[x].type()!=typeid(std::string))
            throw SyntaxError("invalid syntax");
        const auto name=std::any_cast<std::string>(raw_expression_[x]);
        if(!is_variable(name))
            throw SyntaxError("invalid syntax");
        parameter.push_back(name);
        if(para_dict.contains(name))
            throw SyntaxError("duplicate argument \'"+name+"\' in function definition");
        para_dict[name]=make_ptr(name);
    }
}
inline POINTER executor_block(const std::vector<std::pair<int,std::vector<std::any>>> &CODE_BLOCK,const int state,
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
                throw SyntaxError(R"('continue' not properly in loop)");
            i=running_code.back().pre_pos-1;
            now=running_code.back().indentation_count;
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::BREAK)
        {
            while(!running_code.empty()&&running_code.back().pre_state!=RUNNING_STATE::SUCCESSFUL_WHILE)
                running_code.pop_back();
            if(running_code.empty())
                throw SyntaxError(R"('break' outside loop)");
            now=running_code.back().indentation_count;
            running_code.back().pre_state=RUNNING_STATE::BROKEN_WHILE;
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::RETURN)
        {
            if(!state)
                throw SyntaxError(R"('return' outside function)");
            RETURN=1;
            if(LS[rt]==-1)
                RES[rt]=make_ptr(NoneType());
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
                    throw SyntaxError("invalid syntax");
                if(raw_expression_[x].type()==typeid(OPERATOR))
                {
                    const auto now_ope=std::any_cast<OPERATOR>(raw_expression_[x]);
                    if(now_ope==OPERATOR::COMMA)
                        vec.push_back(LS[x]),vec.push_back(RS[x]);
                    else
                        throw SyntaxError("invalid syntax");
                }
                else if(raw_expression_[x].type()==typeid(std::string))
                {
                    const auto now_str=std::any_cast<std::string>(raw_expression_[x]);
                    if(BIND_DICT.contains(now_str))
                    {
                        if(MY_DICT.contains(now_str))
                            throw SyntaxError(R"(name ')"+now_str+R"(' is used before global declaration)");
                        if(BIND_DICT[now_str]==1)
                            throw SyntaxError(R"(name ')"+now_str+R"(' is nonlocal and global)");
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
                    throw SyntaxError("invalid syntax");
                if(raw_expression_[x].type()==typeid(OPERATOR))
                {
                    const auto now_ope=std::any_cast<OPERATOR>(raw_expression_[x]);
                    if(now_ope==OPERATOR::COMMA)
                        vec.push_back(LS[x]),vec.push_back(RS[x]);
                    else
                        throw SyntaxError("invalid syntax");
                }
                else if(raw_expression_[x].type()==typeid(std::string))
                {
                    const auto now_str=std::any_cast<std::string>(raw_expression_[x]);
                    if(!NONLOCAL_DICT->contains(now_str))
                        throw SyntaxError(R"(no binding for nonlocal ')"+now_str+R"(' found)");
                    if(BIND_DICT.contains(now_str))
                    {
                        if(MY_DICT.contains(now_str))
                            throw SyntaxError(R"(name ')"+now_str+R"(' is used before nonlocal declaration)");
                        if(BIND_DICT[now_str]==2)
                            throw SyntaxError(R"(name ')"+now_str+R"(' is nonlocal and global)");
                    }
                    else
                        BIND_DICT[now_str]=1;
                }
            }
        }
        else
        {
            if(const auto tmp_res=executor_single(rt); tmp_res->type()!=typeid(NoneType)&&
                !state&&INTERACTIVE_MODE)
                inner_scope::__print__(tmp_res);
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
            assign(func_name,make_ptr(now_func));
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
                throw IndentationError("unexpected indent");
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
        if(raw_expression_[rt].type()==typeid(KEYWORD)&&std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::IF)
        {
            const bool check=static_cast<bool>(inner_scope::__bool__(executor_single(LS[rt],false)));
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
                    throw IndentationError(R"(expected an indented block after 'if' statement)");
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
                throw SyntaxError("invalid syntax");
            if(pre_state==RUNNING_STATE::FAILED_IF||pre_state==RUNNING_STATE::FAILED_ELIF)
            {
                const bool check=static_cast<bool>(inner_scope::__bool__(executor_single(LS[rt],false)));
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
                        throw IndentationError(R"(expected an indented block after 'elif' statement)");
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
                        throw SyntaxError(R"(expected an indented block after 'elif' statement)");
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
                throw SyntaxError("invalid syntax");
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
                        throw IndentationError(R"(expected an indented block after 'else' statement)");
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
                        throw IndentationError(R"(expected an indented block after 'else' statement)");
                    running_code.emplace_back(CODE_BLOCK[i+1].first,i+1,RUNNING_STATE::NORMAL);
                }
            }
        }
        else if(raw_expression_[rt].type()==typeid(KEYWORD)&&
            std::any_cast<KEYWORD>(raw_expression_[rt])==KEYWORD::WHILE)
        {
            const bool check=static_cast<bool>(inner_scope::__bool__(executor_single(LS[rt],false)));
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
                    throw IndentationError(R"(expected an indented block after 'while' statement)");
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
                throw SyntaxError("invalid syntax");
            if(LS[LS[rt]]==-1||raw_expression_[LS[LS[rt]]].type()!=typeid(std::string))
                throw SyntaxError("invalid syntax");
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
                    throw IndentationError(R"(expected an indented block after 'def' statement)");
                running_code.emplace_back(CODE_BLOCK[i+1].first,i+1,RUNNING_STATE::FUNCTION_CONTENT);
            }
            assign(name,make_ptr(now_func));
            func_name=name;
        }
        else
        {
            brief_single(i,rt);
            if(RETURN)
                return RES[rt];
        }
    }
    return make_ptr(NoneType());
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
        /*
         *All inner functions and classes are defined and initialized there.
         */
        initialize=1;
        assign("type",make_ptr(TYPE::Type));
        assign("int",make_ptr(TYPE::Int));
        assign("float",make_ptr(TYPE::Float));
        assign("bool",make_ptr(TYPE::Bool));
        assign("str",make_ptr(TYPE::Str));
        assign("tuple",make_ptr(TYPE::Tuple));
        assign("complex",make_ptr(TYPE::Complex));
        assign("type",make_ptr(TYPE::Type));
        assign("print",make_ptr(BUILTIN_FUNCTION_OR_METHOD::PRINT));
        assign("input",make_ptr(BUILTIN_FUNCTION_OR_METHOD::INPUT));
        assign("min",make_ptr(BUILTIN_FUNCTION_OR_METHOD::MIN));
        assign("max",make_ptr(BUILTIN_FUNCTION_OR_METHOD::MAX));
        assign("abs",make_ptr(BUILTIN_FUNCTION_OR_METHOD::ABS));
        assign("sum",make_ptr(BUILTIN_FUNCTION_OR_METHOD::SUM));
        assign("eval",make_ptr(BUILTIN_FUNCTION_OR_METHOD::EVAL));
        assign("iter",make_ptr(BUILTIN_FUNCTION_OR_METHOD::ITER));
        assign("next",make_ptr(BUILTIN_FUNCTION_OR_METHOD::NEXT));
    }
    std::exception_ptr exc=nullptr;
    try
    {
        if(s=="begin")
            state=1;
        else if(s=="end")
        {
            if(!ok)
            {
                if(IN_EOF)
                    mode=0,throw SyntaxError("unexpected EOF while parsing");
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
    catch(...){exc=std::current_exception();}
    CODE_BLOCK.clear(),state=0,ok=1;
    if(exc!=nullptr)
        std::rethrow_exception(exc);
    return 0;
}

#endif