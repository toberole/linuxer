#ifndef __BOOK_H__
#define __BOOK_H__


#include <string>

class Book{
    public:
        Book(std::string name);
        ~Book();
    public:
        std::string name;
};

#endif