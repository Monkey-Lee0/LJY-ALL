#ifndef INTELLIGENT_H
#define INTELLIGENT_H

inline bool INTELLIGENT=false;

constexpr int M_KEY_COUNT=14;

inline std::ostream &intelligent(std::ostream &out){INTELLIGENT=true;return out;}
inline std::ostream &dull(std::ostream &out){INTELLIGENT=false;return out;}

inline std::istream &intelligent(std::istream &in){INTELLIGENT=true;return in;}
inline std::istream &dull(std::istream &in){INTELLIGENT=false;return in;}

class invalid_expression final : public std::exception
{
private:
    std::string name;
public:
    invalid_expression()=default;
    explicit invalid_expression(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

class undefined_behavior final : public std::exception
{
private:
    std::string name;
public:
    undefined_behavior()=default;
    explicit undefined_behavior(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

class indentation_error final : public std::exception
{
private:
    std::string name;
public:
    indentation_error()=default;
    explicit indentation_error(std::string a):name(std::move(a)){}
    [[nodiscard]] const char* what() const noexcept override{return name.c_str();}
};

inline bool is_blank(const char s){return s==' '||s=='\t';}
inline bool is_number(const char s){return s>='0'&&s<='9';}
inline bool is_lowercase(const char s){return s>='a'&&s<='z';}
inline bool is_capital(const char s){return s>='A'&&s<='Z';}
inline bool is_letter(const char s){return is_lowercase(s)||is_capital(s);}


#endif