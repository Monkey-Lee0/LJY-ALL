#ifndef PYTHON_H
#define PYTHON_H

#include "parser.h"
#include "iostream"

inline void python()
{
    std::cerr<<"Choose your mode:"<<std::endl;
    std::cerr<<"1.Interactive mode."<<std::endl;
    std::cerr<<"2.code mode."<<std::endl;
    int op;
    std::cin>>op;
    std::string s;
    getline(std::cin,s);
    if(op==1)
    {
        INTERACTIVE_MODE=true;
        int state=0;
        while(true)
        {
            try
            {
                if(!state)
                    std::cout<<">>> ";
                else
                    std::cout<<"... ";
                getline(std::cin,s);
                state=interpreter(s);
            }
            catch(invalid_expression &a)
            {
                std::cout<<"invalid_expression : ";
                state=0;
                puts(a.what());
            }
            catch(std::runtime_error &a)
            {
                std::cout<<"runtime_error : ";
                state=0;
                puts(a.what());
            }
            catch(std::domain_error &a)
            {
                std::cout<<"domain_error : ";
                state=0;
                puts(a.what());
            }
            catch(undefined_behavior &a)
            {
                std::cout<<"undefined_behavior : ";
                state=0;
                puts(a.what());
            }
            catch(std::bad_any_cast &a)
            {
                std::cout<<"bad_any_cast : ";
                state=0;
                puts(a.what());
            }
            catch(indentation_error &a)
            {
                std::cout<<"indentation_error : ";
                state=0;
                puts(a.what());
            }
            catch(...)
            {
                puts("What's wrong with the program (and your brain)?");
                state=0;
            }
        }
    }
    interpreter("begin");
    while(true)
    {
        getline(std::cin,s);
        interpreter(s);
        if(s=="end")
            break;
    }
}

#endif