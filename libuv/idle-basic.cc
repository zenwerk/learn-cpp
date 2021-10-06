#include <iostream>
#include <uv.h>

int64_t counter = 0;
void wait_for_a_while(uv_idle_t* handle) {
    counter++;

    if (counter >= (int64_t)50e5)
        uv_idle_stop(handle); // イベントループの管理リストからハンドルを削除
    if (counter % (uint64_t)10e5 == 0)
        std::cout << counter << std::endl;
}

int main() {
    uv_idle_t idler; // ハンドルオブジェクト

    uv_idle_init(uv_default_loop(), &idler); // ループの初期化   (Idleハンドルのセットアップ)

    uv_idle_start(&idler, wait_for_a_while); // コールバックの登録(Idleハンドルのセットアップ)

    std::cout << "Idling..." << std::endl;
    uv_run(uv_default_loop(), UV_RUN_DEFAULT); // イベントループの開始

    uv_loop_close(uv_default_loop()); // 終了処理
    return 0;
}
