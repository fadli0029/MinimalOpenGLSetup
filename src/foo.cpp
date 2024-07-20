#include "foo/foo.hpp"

Foo::Foo(const std::string& name) : name_(name) {}

void Foo::printName() const {
    std::cout << "Foo name: " << name_ << std::endl;
}
