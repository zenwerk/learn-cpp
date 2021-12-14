#include <string>
#include <iostream>
#include "hello2.hpp"

struct SemanticAction {
    void syntax_error() {}

    void stack_overflow() {}

    /*
     * 各非終端記号の型 (たとえば HelloWorld<T>) から、Parserのインスタンス化のときに
     * テンプレートパラメータの第1引数として与える「値集合すべてをあらわす型」への変換を提供する関数。
     * セマンティックアクション関数がひとつでもある場合、
     * 安全な型変換のために upcast() を定義しなければならない。
     */
    void upcast(std::string &parser_value, const std::string &result_of_semantic_action) {
        /* 値集合の型 <- セマンティックアクションの結果の型 */
        parser_value = result_of_semantic_action;
    }

    /*
     * テンプレートパラメータの第1引数として与える「値集合すべてをあらわす型(今回の例では`std::string`)」から
     * 各トークンの個別の型への安全な変換を提供する関数
     * `%token` で型を紐づけたら必要になる関数
     * upcast の反対の動作
     */
    void downcast(std::string &value_of_semantic_action, const std::string &parser_value) {
        /* セマンティックアクションの型 <- 値集合の型 */
        value_of_semantic_action = parser_value;
    }

    // x -> `token_Hello` に関連付けられた値、y -> `token_World` に関連付けられた値
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