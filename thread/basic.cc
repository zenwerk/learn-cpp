#include <iostream>
#include <string>
#include <thread>
#include <mutex>

// 排他的出力処理
std::mutex printMtx_;

void print(const std::string &s) {
    std::lock_guard<std::mutex> lk{printMtx_};
    std::cout << s << std::endl;
}

// 関数オブジェクト
struct Foo {
    void operator()() {
        print("別スレッドから実行されています。");
    }
};

void f1(int n) {
    print("f1: " + std::to_string(n) + "が渡されました");
}

void f2(int& n) {
    print("f2: " + std::to_string(n) + "が渡されました");
}

void f3(std::unique_ptr<int> pn) {
    print("f3: " + std::to_string(*pn) + "が渡されました");
}

void stop(int n) {
    std::this_thread::sleep_for(std::chrono::seconds(n));
    print(std::to_string(n) + "秒待ちました。");
}


int main() {
    int x = 10, y = 20;
    std::unique_ptr<int> z{new int(30)};
    Foo foo;

    // 並列実行可能数 != 同時実行可能数
    std::cout << "並列実行可能なスレッド数は " << std::thread::hardware_concurrency() << " です" << std::endl;

    std::thread th(foo);
    // Thread実行される関数に対して初期化時に(コピーして)引数を渡せる
    std::thread th1{f1, x}; // 値渡し
    std::thread th2{f2, std::ref(y)}; // 参照型渡し
    std::thread th3{f3, std::move(z)}; // move

    std::thread oneSecond{stop, 1}; // 1秒かかる処理
    std::thread oneMinute{stop, 60}; // 1分かかる処理

    // スレッドの終了待機
    th.join();
    th1.join();
    th2.join();
    th3.join();

    // スレッドの終了を待機可能か？
    std::cout << "oneSecond.joinable?: " << oneSecond.joinable() << std::endl;
    oneSecond.join();
    std::cout << "oneSecond.joinable?: " << oneSecond.joinable() << std::endl;

    // 時間がかかるのでスレッドを破棄
    std::cout << "oneMinute.joinable?: " << oneMinute.joinable() << std::endl;
    oneMinute.detach(); // 管理を破棄するだけで oneMinute は裏で実行中なので注意
    std::cout << "oneMinute.joinable?: " << oneMinute.joinable() << std::endl;

    return 0;
}