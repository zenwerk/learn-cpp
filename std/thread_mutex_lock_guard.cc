#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx_; // 共通排他処理のため Mutex


void worker(int n) {
    using clock = std::chrono::steady_clock;
    auto start_point_ = clock::now();

    std::lock_guard<std::mutex> lk{mtx_}; // ロックを獲得
    std::this_thread::sleep_for(std::chrono::seconds{n});

    auto elapsed_time_ = clock::now() - start_point_;
    std::cout << "Elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(elapsed_time_).count() << " seconds" << std::endl;
}


int main() {
    std::thread th1{worker, 3}; // 3秒待機するスレッドを起動

    std::this_thread::sleep_for(std::chrono::milliseconds{10}); // 少し待機

    std::thread th2{worker, 1}; // 1秒待機するスレッドを起動

    th1.join(); // 起動から終了まで3秒かかる
    th2.join(); // ロックを待つので最低でも起動から終了まで3秒かかる

    return 0;
}