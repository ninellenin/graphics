#ifndef MYEXCEPTION_H
#define MYEXCEPTION_H

#include <exception>
#include <string>

class MyException : std::exception {
private:
    std::string what_;
public:
    MyException() throw();
    MyException (const std::string &what) throw();
    const char* what() const throw();
    ~MyException() throw();
};

#endif // MYEXCEPTION_H
