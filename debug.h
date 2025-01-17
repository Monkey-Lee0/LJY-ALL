#ifndef DEBUG_H
#define DEBUG_H

#include"type.h"

inline std::string type_name_D(const POINTER &x)
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
    if(x->type()==typeid(OPERATOR))
        return "operator";
    if(x->type()==typeid(std::string))
        return "variable";
    if(x->type()==typeid(POINTER))
        return "pointer";
    if(x->type()==typeid(DICT))
        return "dict";
    if(x->type()==typeid(DICT_PTR))
        return "dict_ptr";
    if(x->type()==typeid(Tuple_iterator))
        return "Tuple_iterator";
    if(x->type()==typeid(str_ascii_iterator))
        return "str_ascii_iterator";
    return "unknown";
}

inline void print_D(const std::any &a)
{
    if(a.type()==typeid(_int))
        std::cerr<<std::any_cast<_int>(a);
    if(a.type()==typeid(_float))
        std::cerr<<std::setprecision(12)<<std::any_cast<_float>(a);
    if(a.type()==typeid(bool))
        std::cerr<<(std::any_cast<bool>(a)?"True":"False");
    if(a.type()==typeid(str))
    {
        auto tmp=std::string(std::any_cast<str>(a));
        int op1=0,op2=0;
        for(const auto t:tmp)
            if(t=='\'')
                op1=1;
            else if(t=='\"')
                op2=1;
        if(!op1||op2)
            std::cerr<<"\'"+tmp+"\'";
        else
            std::cerr<<"\""+tmp+"\"";
    }
    if(a.type()==typeid(Tuple))
    {
        const auto tpl=std::any_cast<Tuple>(a);
        std::cerr<<"(";
        if(tpl.SIZE())
        {
            print_D(tpl.__getitem__(0));
            for(int i=1;i<tpl.SIZE();i++)
                std::cerr<<", ",print_D(tpl.__getitem__(i));
            if(tpl.SIZE()==1)
                std::cerr<<",";
        }
        std::cerr<<")";
    }
    if(a.type()==typeid(OPERATOR))
        std::cerr<<"OPERATOR-"<<operator_what(std::any_cast<OPERATOR>(a));
    if(a.type()==typeid(python_function))
        std::cerr<<"function"<<std::any_cast<python_function>(a).id;
    if(a.type()==typeid(NoneType))
        std::cerr<<"None";
    if(a.type()==typeid(std::string))
        std::cerr<<"variable-"<<std::any_cast<std::string>(a);
    if(a.type()==typeid(KEYWORD))
        std::cerr<<"KEYWORD-"<<keyword_what(std::any_cast<KEYWORD>(a));
    if(a.type()==typeid(Tuple_iterator))
        std::cerr<<"tuple_iterator";
    if(a.type()==typeid(str_ascii_iterator))
        std::cerr<<"str_ascii_iterator";
}

inline void print_raw_D(const std::vector<std::any> &raw)
{
    std::cerr<<"===print_raw==="<<std::endl;
    for(const auto& t:raw)
        print_D(t),std::cerr<<" ";
    std::cerr<<std::endl;
    std::cerr<<"===end_print_raw==="<<std::endl;
}

inline void print_tree_D(const std::vector<std::any> &raw,const std::vector<int> &LS,const std::vector<int> &RS,
    const std::vector<int> &FA,const int rt)
{
    std::vector<int> sta;
    sta.push_back(rt);
    std::cerr<<"===print_tree==="<<std::endl;
    while(!sta.empty())
    {
        const int x=sta.back();sta.pop_back();
        print_D(raw[x]);
        std::cerr<<" "<<x<<" "<<LS[x]<<" "<<RS[x]<<" "<<FA[x]<<std::endl;
        if(RS[x]!=-1)
            sta.push_back(RS[x]);
        if(LS[x]!=-1)
            sta.push_back(LS[x]);
    }
    std::cerr<<"===end_print_tree=="<<std::endl;
}

#endif