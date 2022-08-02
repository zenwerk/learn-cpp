#include <chrono>
#include <cmath>
#include <iostream>

#ifdef WIN32
#define M_PI 3.141592653589793
#else
#include <ctime>
#endif

// from: https://gist.github.com/mcleary/b0bf4fa88830ff7c882d
class Timer {
public:
    void start() {
        m_StartTime = std::chrono::steady_clock::now();
        m_bRunning = true;
    }

    void stop() {
        m_EndTime = std::chrono::steady_clock::now();
        m_bRunning = false;
    }

    long long elapsedMilliseconds() {
        std::chrono::time_point<std::chrono::steady_clock> endTime;

        if (m_bRunning) {
            endTime = std::chrono::steady_clock::now();
        } else {
            endTime = m_EndTime;
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
    }

    long long elapsedSeconds() {
        return elapsedMilliseconds() / 1000;
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> m_StartTime;
    std::chrono::time_point<std::chrono::steady_clock> m_EndTime;
    bool m_bRunning = false;
};


// from: https://codereview.stackexchange.com/a/225927
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

    long long GetElapsedSeconds() {
        auto elapsed = GetElapsed();
        return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    }
};


long fibonacci(unsigned n) {
    if (n < 2) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
//    std::chrono::time_point<std::chrono::steady_clock> start, end;
//    start = std::chrono::steady_clock::now();
//    Timer timer;
//    timer.start();
//    std::cout << "f(42) = " << fibonacci(42) << '\n';
//    timer.stop();
//
//    std::cout << "Time: " << timer.elapsed() << std::endl;
//    end = std::chrono::steady_clock::now();

//    std::chrono::duration<double> elapsed_seconds = end-start;
//    std::time_t end_time = std::chrono::steady_clock::to_time_t(end);

//    std::cout << "finished computation at " << std::ctime(&end_time)
//    << "elapsed time: " << elapsed_seconds.count() << "s\n";

    Timer timer;
    timer.start();
    int counter = 0;
    double test, test2;
    while (timer.elapsedSeconds() < 2.0) {
        counter++;
        test = std::cos(counter / M_PI);
        test2 = std::sin(counter / M_PI);
    }
    timer.stop();

    std::cout << counter << std::endl;
    std::cout << "Seconds: " << timer.elapsedSeconds() << std::endl;
    std::cout << "Milliseconds: " << timer.elapsedMilliseconds() << std::endl;

    //--------------------
    SimpleTimer stimer;
    counter = 0;
    stimer.Start();
    while (stimer.GetElapsedSeconds() < 2.0) {
        counter++;
    }
    stimer.Stop();
    std::cout << counter << std::endl;
    std::cout << "Seconds: " << stimer.GetElapsedSeconds() << std::endl;

    return 0;
}