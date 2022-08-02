#include <iostream>
#include <thread>

class SimpleTimer {
    using clock = std::chrono::steady_clock;
    clock::time_point start_time_ = {};
    clock::duration elapsed_time_ = {};

public:
    [[nodiscard]] bool IsRunning() const {
        return start_time_ != clock::time_point{};
    }

    void Start() {
        if (!IsRunning()) {
            start_time_ = clock::now();
        }
    }

    void Stop() {
        if (IsRunning()) {
            elapsed_time_ += clock::now() - start_time_;
            start_time_ = {};
        }
    }

    void Reset() {
        start_time_ = {};
        elapsed_time_ = {};
    }

    clock::duration GetElapsed() {
        auto result = elapsed_time_;
        if (IsRunning()) {
            result += clock::now() - start_time_;
        }
        return result;
    }

    long long GetElapsedMilliSeconds() {
        auto elapsed = GetElapsed();
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    }

    long long GetElapsedSeconds() {
        auto elapsed = GetElapsed();
        return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    }
};

void Ticker() {
    auto timer = SimpleTimer();

    timer.Start();
    while (timer.GetElapsedSeconds() < 6) {
        std::this_thread::sleep_for(std::chrono::milliseconds{1000});
        std::cout << "Elapsed " << timer.GetElapsedMilliSeconds() << " milli seconds" << std::endl;
        if (timer.GetElapsedMilliSeconds() % 1000 == 0) {
            std::cout << "Elapsed " << timer.GetElapsedSeconds() << " seconds" << std::endl;
        }
    }
}

int main() {
    std::thread th(Ticker);

    th.join();

    return 0;
}