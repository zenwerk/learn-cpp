#include <iostream>
#include <mutex>
#include <deque>


template<class T>
struct LockedQueue {
    explicit LockedQueue(size_t capacity) : capacity_(capacity) {}

    void enqueue(const T& x) {
        std::unique_lock<std::mutex> lock{m_};
        // 通知が送られてくるまで wait する. 容量に空きがないときは Lock する
        cv_enqueue_.wait(lock, [this]{ return data_.size() != capacity_; });
        data_.push_back(x);
        // Dequeue側のいずれかのLockに通知
        cv_dequeue_.notify_one();
    }

    T dequeue() {
        std::unique_lock<std::mutex> lock{m_};
        // lock を解除し, 通知が送られてくるまで wait する. 空なら Dequeue処理を Lock
        cv_dequeue_.wait(lock, [this]{ return !data_.empty(); });
        T ret = data_.front();
        data_.pop_front();
        // Enqueue側のいずれのかのLockに通知
        cv_enqueue_.notify_one();
        return ret;
    }

private:
    std::mutex m_;
    std::deque<T> data_;
    size_t capacity_;
    std::condition_variable cv_enqueue_;
    std::condition_variable cv_dequeue_;
};

void worker(LockedQueue<int>& lq) {
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