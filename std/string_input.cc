#include <string>
#include <iostream>


int main() {
    std::cout << "Please enter a string:";
    std::string name;

    std::getline(std::cin, name);
    std::cout << "Hello, " << name << std::endl;

    return 0;
}