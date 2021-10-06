#include <iostream>

#include <uv.h>
#include <RtMidi.h>

#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

void _sleep(int milliseconds) {
#ifdef _WINDOWS
    Sleep(milliseconds);
#else
    sleep(miliseconds);
#endif
}

int64_t counter = 0;
void wait_for_a_while(uv_idle_t* handle) {
    counter++;

    auto mout = (RtMidiOut *)handle->data;

    if (counter >= 10000000)
        uv_idle_stop(handle); // イベントループの管理リストからハンドルを削除
/*
    if (counter % 1000000 == 0) {
        std::vector<unsigned char> message;
        // Send out a series of MIDI messages.
        // Program change: 192, 5
        message.push_back(192);
        message.push_back(5);
        mout->sendMessage(&message);
        // Control Change: 176, 7, 100 (volume)
        message[0] = 176;
        message[1] = 7;
        message.push_back(100);
        mout->sendMessage(&message);
        // Note On: 144, 64, 90
        message[0] = 144;
        message[1] = 64;
        message[2] = 90;
        mout->sendMessage(&message);
        _sleep(500); // Platform-dependent ... see example in tests directory.
        // Note Off: 128, 64, 40
        message[0] = 128;
        message[1] = 64;
        message[2] = 40;
        mout->sendMessage(&message);
        std::cout << "Midi OUT!" << std::endl;
    }
*/
}

int main() {
    uv_idle_t idler; // ハンドルオブジェクト

    std::unique_ptr<RtMidiOut> mout = nullptr;
    // RtMidiOut constructor
    try {
        std::unique_ptr<RtMidiOut> _mout(new RtMidiOut());
        mout = std::move(_mout);
    }
    catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }
    unsigned int nPorts = mout->getPortCount();
    if (nPorts == 0) {
        std::cout << "No ports available!\n";
        exit(EXIT_FAILURE);
    }
    // Open first available port.
    try {
        mout->openPort(0);
    }
    catch (RtMidiError &error) {
        error.printMessage();
        exit(EXIT_FAILURE);
    }

    idler.data = &mout;
    uv_idle_init(uv_default_loop(), &idler); // ループの初期化   (Idleハンドルのセットアップ)

    // start はハンドルの有効化(activate)を意味する
    // idler が指す uv_loop_t が管理する Idleハンドル管理リストに idler が追加される
    uv_idle_start(&idler, wait_for_a_while); // コールバックの登録(Idleハンドルのセットアップ)

    std::cout << "Idling...\n";

    uv_run(uv_default_loop(), UV_RUN_DEFAULT); // イベントループの開始

    std::cout << "Exit...\n";

    uv_loop_close(uv_default_loop()); // 終了処理
    return 0;
}
