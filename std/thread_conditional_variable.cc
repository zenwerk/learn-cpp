#include <iostream>
#include <deque>
#include <thread>
#include <mutex>


template<class T>
struct LockedQueue {
    explicit LockedQueue(size_t capacity) : capacity_(capacity) {}

    void enqueue(const T &x) {
        std::unique_lock<std::mutex> lock{m_}; // ロックを獲得
        // ロックを解除し通知が送られてくるまで wait する. 「容量に空きがある」まで待機
        cv_enqueue_.wait(lock, [this]{ return data_.size() != capacity_; });
        data_.push_back(x);
        // Dequeue側のいずれかの条件変数に「空キューではなくなった」と通知する
        cv_dequeue_.notify_one();
    }

    T dequeue() {
        std::unique_lock<std::mutex> lock{m_}; // ロックを獲得
        // lock を解除し, 通知が送られてくるまで wait する. 「空ではない」まで待機する
        cv_dequeue_.wait(lock, [this]{ return !data_.empty(); });
        T ret = data_.front();
        data_.pop_front();
        // Enqueue側のいずれのかの条件変数に「容量に空きができた」と通知する
        cv_enqueue_.notify_one();
        return ret;
    }

private:
    std::mutex m_; // 条件変数は同じ Mutex を使う必要があるので注意
    std::deque<T> data_;
    size_t capacity_;
    std::condition_variable cv_enqueue_;
    std::condition_variable cv_dequeue_;
};

void worker(LockedQueue<int> &lq) {
    for (int i = 0; i < 5; i++) {
        lq.enqueue(i);
        std::cout << "Enqueued: " << i << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
}

int main() {
    LockedQueue<int> lq(2);

    std::thread th(worker, std::ref(lq)); // woker thread で 1～5までEnqueue

    std::this_thread::sleep_for(std::chrono::seconds{1});

    for (int i = 0; i < 5; i++) {
        int n = lq.dequeue();
        std::cout << "Popped: " << n << std::endl;
    }

    th.join();

    return 0;
}