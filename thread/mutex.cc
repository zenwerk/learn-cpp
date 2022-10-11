#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

struct Data {
    std::mutex mtx; // 排他ロックMutex
    int data = 0;
};

void worker(Data &d) {
    d.mtx.lock(); // 排他的ロックを取得

    // 時間のかかる処理
    int n = d.data;
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    d.data = n + 1;

    d.mtx.unlock(); // 排他的ロックを解放
}

int main() {
    std::vector<std::thread> ths(4);
    Data d; // スレッドで共有されるデータ

    // WorkerThreadを4つ起動
    for (std::thread& th : ths) {
        th = std::thread{worker, std::ref(d)};
    }

    for (std::thread& th : ths) {
        th.join();
    }

    std::cout << "d.data: " << d.data << std::endl;

    return 0;
}