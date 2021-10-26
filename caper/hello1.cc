#include <iostream>
#include "hello1.hpp"

struct SemanticAction {
    void syntax_error() {}

    void stack_overflow() {}

    /*
     * 各非終端記号の型 (たとえば HelloWorld<T>) から、Parserのインスタンス化のときに
     * テンプレートパラメータの第1引数として与える「値集合すべてをあらわす型」への変換を提供する関数。
     * セマンティックアクション関数がひとつでもある場合、
     * 安全な型変換のために upcast() を定義しなければならない。
     */
    /*
     * 今回は文法ファイルで定義したGreetの受理結果は `HelloWorld<int>` なので
     * パーサの第1テンプレートパラメータとして指定した値集合全体の型もintなので以下の1行定義でOK
     */
    void upcast(int &to, int from) { to = from; /* 値集合の型 <- セマンティックアクションの結果の型 */}

    // セマンティックアクション 'Greet' の定義
    int Greet() {
        std::cout << "hello world" << std::endl;
        return 0;
    }
};

int main(int, char **) {
    SemanticAction sa;
    hello_world::Parser<int, SemanticAction> parser(sa);

    parser.post(hello_world::token_Hello, 0);
    parser.post(hello_world::token_World, 0);
    parser.post(hello_world::token_eof, 0);

    return 0;
}