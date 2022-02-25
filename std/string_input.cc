#include <string>
#include <iostream>

int main() {
    std::cout << "Please enter a string." << std::endl;
    std::string input;

    while (std::getline(std::cin, input)) {
        if (std::cin.fail() || std::cin.eof()) {
            std::cin.clear();
        }
        std::cout << "INPUT: " << input << std::endl;
    }

    return 0;
}