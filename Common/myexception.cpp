#include "myexception.h"

MyException::MyException() throw()
{
    std::exception();
}

MyException::MyException (const std::string &what) throw() :
    what_(what)
{
}

const char* MyException::what() const throw()
{
    return what_.c_str();
}

MyException::~MyException() throw()
{

}
