#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>

int value = 0;
std::shared_mutex smtx_;
std::mutex pmtx;
std::condition_variable cv;

void print(const std::string &s) {
    std::lock_guard<std::mutex> lk{pmtx};
    std::cout << s << std::endl;
}

void writer(int n) {
    print("Writer - try lock_guard");
    std::lock_guard<std::shared_mutex> lk{smtx_}; // ユニークなロックを獲得
    std::this_thread::sleep_for(std::chrono::seconds{1}); // 1秒待機
    value = n;
    print("Writer - unlock_guard");
}

void shared_lock_reader(int &n) {
    print("Shared Lock Reader - try shared_lock");
    std::shared_lock<std::shared_mutex> lk{smtx_}; // 共有ロックを取得
    std::this_thread::sleep_for(std::chrono::seconds{1}); // 1秒待機
    n = value;
    print("Shared Lock Reader - unlock_shared");
}

void unique_lock_reader(int &n) {
    print("Unique Lock Reader - try lock_guard");
    std::lock_guard<std::shared_mutex> lk{smtx_}; // ユニークなロックを取得
    std::this_thread::sleep_for(std::chrono::seconds{1}); // 1秒待機
    n = value;
    print("Unique Lock Reader - unlock_guard");
}

void read_write_with_shared_lock() {
    std::thread th1{writer, 3};

    std::this_thread::sleep_for(std::chrono::milliseconds{10}); // 少し待機

    int v1, v2, v3;
    std::thread th2{shared_lock_reader, std::ref(v1)};
    std::thread th3{shared_lock_reader, std::ref(v2)};
    std::thread th4{shared_lock_reader, std::ref(v3)};

    th1.join();
    th2.join();
    th3.join();
    th4.join();

    std::cout << "-----------" << std::endl;
    std::cout << "value: " << value << std::endl;
    std::cout << "   v1: " << v1 << std::endl;
    std::cout << "   v2: " << v2 << std::endl;
    std::cout << "   v3: " << v3 << std::endl;
}

void read_write_with_unique_lock() {
    std::thread th1{writer, 7};

    std::this_thread::sleep_for(std::chrono::milliseconds{10}); // 少し待機

    int v1, v2, v3;
    std::thread th2{unique_lock_reader, std::ref(v1)};
    std::thread th3{unique_lock_reader, std::ref(v2)};
    std::thread th4{unique_lock_reader, std::ref(v3)};

    th1.join();
    th2.join();
    th3.join();
    th4.join();

    std::cout << "-----------" << std::endl;
    std::cout << "value: " << value << std::endl;
    std::cout << "   v1: " << v1 << std::endl;
    std::cout << "   v2: " << v2 << std::endl;
    std::cout << "   v3: " << v3 << std::endl;
}

int main() {
    using clock = std::chrono::steady_clock;

    std::cout << "## Reader-Writer pattern with Unique Lock" << std::endl;
    auto start_point_ = clock::now();
    read_write_with_unique_lock();
    auto elapsed_time_ = clock::now() - start_point_;
    std::cout << "Elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(elapsed_time_).count() << " seconds" << std::endl;

    std::cout << std::endl;

    std::cout << "## Reader-Writer pattern with Shared Lock" << std::endl;
    start_point_ = clock::now();
    read_write_with_shared_lock();
    elapsed_time_ = clock::now() - start_point_;
    std::cout << "Elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(elapsed_time_).count() << " seconds" << std::endl;

    return 0;
}