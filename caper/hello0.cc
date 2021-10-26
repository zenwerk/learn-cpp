#include <iostream>
#include "hello0.hpp"

// セマンティックアクション-ハンドラ
// テンプレートパラメータとしてパーサに渡す(classでも可)
struct SemanticAction {
    // 文法エラーハンドラのための以下の2つの関数の定義は必須
    void syntax_error() {}

    void stack_overflow() {}
};

int main(int, char **) {
    SemanticAction sa;

    // パーサーインスタンスの生成
    // テンプレートの第1引数は token に紐づけられる型

    /* Parserクラステンプレートの第3引数'StackSize'を省略しています。
     * 第3引数は整数で、デフォルト値は「STL使用」なら0、「STL不使用」なら'適当な数値'(1024)になっています。
     * 「STL使用」のときに'StackSize'を0にすると、決してオーバーフローしません
     * (メモリがなくなった時点でメモリ割り当て失敗の例外が発生するでしょう)。 */
    hello_world::Parser<int, SemanticAction> parser(sa);

    // token を push (caperはプッシュパーサー)
    parser.post(hello_world::token_Hello, 0);
    parser.post(hello_world::token_World, 0);
    parser.post(hello_world::token_eof, 0);

    return 0;
}