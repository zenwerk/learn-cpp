#include <string>
#include <iostream>
#include "hello2.hpp"

struct SemanticAction {
    void syntax_error() {}

    void stack_overflow() {}

    void upcast(std::string &x, const std::string &y) { x = y; }

    // ParserのValue型から各記号の個別の型への安全な変換を提供する関数
    // upcast の反対の動作
    void downcast(std::string &x, const std::string &y) { x = y; }

    // xがHelloに関連付けられた値、yがWorldに関連付けられた値
    // 戻り値は、パーサによって、HelloWorldに関連付けられる
    std::string Greet(const std::string &x, const std::string &y) {
        std::cout << x << y << std::endl;
        return "";
    }
};

int main(int, char **) {
    SemanticAction sa;
    hello_world::Parser<std::string, SemanticAction> parser(sa);

    parser.post(hello_world::token_Hello, "Guten Tag, ");
    parser.post(hello_world::token_World, "Welt");
    parser.post(hello_world::token_eof, "");

    return 0;
}