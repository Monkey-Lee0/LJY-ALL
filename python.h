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
            catch(SyntaxError &a)
            {
                std::cout<<"SyntaxError: ";
                state=0;
                puts(a.what());
            }
            catch(NameError &a)
            {
                std::cout<<"NameError: ";
                state=0;
                puts(a.what());
            }
            catch(IndentationError &a)
            {
                std::cout<<"IndentationError: ";
                state=0;
                puts(a.what());
            }
            catch(IndexError &a)
            {
                std::cout<<"IndexError: ";
                state=0;
                puts(a.what());
            }
            catch(ValueError &a)
            {
                std::cout<<"ValueError: ";
                state=0;
                puts(a.what());
            }
            catch(TypeError &a)
            {
                std::cout<<"TypeError: ";
                state=0;
                puts(a.what());
            }
            catch(ZeroDivisionError &a)
            {
                std::cout<<"ZeroDivisionError: ";
                state=0;
                puts(a.what());
            }
            catch(std::bad_any_cast &a)
            {
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