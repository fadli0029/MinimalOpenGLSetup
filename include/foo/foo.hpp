#ifndef FOO_HPP
#define FOO_HPP

#include <iostream>
#include <string>

class Foo {
public:
    Foo(const std::string& name);
    void printName() const;

private:
    std::string name_;
};

#endif // FOO_HPP
