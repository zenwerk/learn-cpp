#include <cstdio>
#include <ctime>
#include <chrono>
#include <iostream>

int main() {
    timespec ts{};

    timespec_get(&ts, TIME_UTC);
    printf("sec=%ld  nanosec=%09ld\n", ts.tv_sec, ts.tv_nsec);
    timespec_get(&ts, TIME_UTC);
    printf("sec=%ld  nanosec=%09ld\n", ts.tv_sec, ts.tv_nsec);

    // 秒未満の値をミリ秒で取得
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::nanoseconds{ts.tv_nsec});
    std::cout << "ms:" << ms.count() << std::endl;

    // 秒以上の値だけを日時フォーマットで出力
    std::cout << std::ctime(&ts.tv_sec) << std::endl;

    return 0;
}