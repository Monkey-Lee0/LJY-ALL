#ifndef EXCEPTION_H
#define EXCEPTION_H

#include<string>

class SyntaxError final : public std::exception
{
    std::string name;
public:
    SyntaxError()=default;
    explicit SyntaxError(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

class TypeError final : public std::exception
{
    std::string name;
public:
    TypeError()=default;
    explicit TypeError(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

class NameError final : public std::exception
{
    std::string name;
public:
    NameError()=default;
    explicit NameError(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

class IndentationError final : public std::exception
{
    std::string name;
public:
    IndentationError()=default;
    explicit IndentationError(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

class IndexError final : public std::exception
{
    std::string name;
public:
    IndexError()=default;
    explicit IndexError(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

class ValueError final : public std::exception
{
    std::string name;
public:
    ValueError()=default;
    explicit ValueError(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

class StopIteration final : public std::exception
{
    std::string name;
public:
    StopIteration()=default;
    explicit StopIteration(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

class ZeroDivisionError final : public std::exception
{
    std::string name;
public:
    ZeroDivisionError()=default;
    explicit ZeroDivisionError(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

#endif