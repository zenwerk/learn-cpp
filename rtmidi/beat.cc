#include <iostream>
#include <atomic>
#include <cstdint>
#include <chrono>
#include <thread>
#include <format>
#include <fstream>

#include "RtMidi.h"


std::atomic<bool> running = false;
const uint32_t midi_clock_base = 24; // MIDI clock base / MIDI クロックの送信基準(MIDI1.0の固定値)
std::atomic<uint32_t> tpqn = 96; // ticks per quarter note / 4部音符に対する分解能 / 古い機器では 24,48 が多い
std::atomic<float> bpm = 120.; // beats per minute
std::atomic<uint32_t> tick = 0; // 1 tick に必要な時間

std::atomic<uint32_t> qn_time = 0.0; // 1拍あたりの時間 (ms)
std::atomic<uint32_t> tick_time = 0.0; // 1 tick に必要な時間 (ms)
std::atomic<uint32_t> f8_interval = 0;


void calc_tick() {
  if (tpqn <= 0 || tpqn % midi_clock_base != 0) {
    std::cerr << "Invalid TPQN value: " << tpqn << std::endl;
    return;
  }
  qn_time = (60.0f / bpm) * 1000000; // BPMから四分音符(1拍)の時間を計算 (micros)
  tick_time = qn_time / tpqn; // 四分音符の時間を分解能(TPQN)で割って 1 tick に必要な時間を計算 (microsec)
  f8_interval = tpqn / midi_clock_base; // 四分音符の分解能から MIDI Clock イベントの送信間隔を計算
}


std::mutex mtx;
std::condition_variable cv;

void sequencer(std::unique_ptr<RtMidiOut> &mout) {
  uint32_t tick_count = 0;
  uint32_t f8_count = 0;
  using clock = std::chrono::steady_clock;
  clock::time_point start_time = {};
  clock::duration elapsed_time = {};

  // wait for ready
  std::unique_lock<std::mutex> lk(mtx);
  cv.wait(lk, [&](){return running.load();});

  calc_tick();

  uint32_t count = 0;

  std::cout << "Start sequencer" << std::endl;
  std::cout << "TPQN: " << tpqn << std::endl;
  std::cout << "BPM: " << bpm << std::endl;
  std::cout << "QN Time: " << qn_time << " microsec" << std::endl;
  std::cout << "Tick Time: " << tick_time << " microsec" << std::endl;
  std::cout << "F8 Interval: " << f8_interval << std::endl;

  bool note_on = false;
  start_time = clock::now();

  //auto now_utc = std::chrono::system_clock::now();
  //std::ofstream ofs("./log_" + std::format("{:%Y%m%d%H%M%S}", now_utc) + ".txt");

  while (running) {
    // 何秒経過したかミリ秒で取得
    elapsed_time = clock::now() - start_time;
    auto elapsed_microsec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_time).count();

    if (elapsed_microsec >= tick_time) {
      // 1 tick 経過
      tick_count++;

      if (tick_count % f8_interval == 0) {
        // MIDI Clock イベントを送信
        tick_count = 0;
        f8_count++;

        if (f8_count == midi_clock_base) {
          count++;
          // 1拍経過
          f8_count = 0;

          // ofs << elapsed_microsec << std::endl;

          // 1拍ごとに ON/OFF を切り替える
          if (note_on) {
            // ここで Node OFF を送信する
            note_on = false;
            auto msg = std::vector<uint8_t>{0x80, 63, 40};
            mout->sendMessage(&msg);
            std::cout << "OFF: " << elapsed_microsec << "μs " << count << std::endl;
          } else {
            // ここで Node ON を送信する
            note_on = true;
            auto msg = std::vector<uint8_t>{0x90, 63, 90};
            mout->sendMessage(&msg);
            std::cout << "ON:  " << elapsed_microsec << "μs " << count << std::endl;
          }
        }
      }
      // 1 tick に必要な時間だけ経過したのでリセット
      start_time = clock::now();
    }
    // CPUビジー回避: 1〜100μsで調整してCPUの負荷を確認する
    std::this_thread::sleep_for(std::chrono::microseconds(50));
  }

  auto msg = std::vector<unsigned char>{0x80, 63, 90};
  mout->sendMessage(&msg);
}


void midiOut(std::unique_ptr<RtMidiOut> &mout) {
  // Check available ports.
  unsigned int nPorts = mout->getPortCount();
  if (nPorts == 0) {
    std::cout << "No ports available!\n";
    return;
  }
  // Open first available port.
  try {
    mout->openPort(0);
  }
  catch (RtMidiError &error) {
    error.printMessage();
    return;
  }

  std::cout << "Setting Midi OUT!" << std::endl;
}


void mycallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
  unsigned int nBytes = message->size();
  for (unsigned int i = 0; i < nBytes; i++)
    std::cout << "Byte " << i << " = " << (int) message->at(i) << ", ";
  if (nBytes > 0)
    std::cout << "stamp = " << deltatime << std::endl;
}


void midiIn(std::unique_ptr<RtMidiIn> &min) {
  std::vector<unsigned char> message;
  // Check available ports.
  unsigned int nPorts = min->getPortCount();
  if (nPorts == 0) {
    std::cout << "No ports available!\n";
    return;
  }
  min->openPort(0);
  min->setCallback(&mycallback);
  // Don't ignore sysex, timing, or active sensing messages.
  min->ignoreTypes(false, false, false);

  std::cout << "\nReading MIDI input ... press any key to quit.\n";
  char input;
  std::cin.get(input);
}


int main() {
  std::unique_ptr<RtMidiIn> min = nullptr;
  std::unique_ptr<RtMidiOut> mout = nullptr;

  // RtMidiIn constructor
  try {
    std::unique_ptr<RtMidiIn> _min(new RtMidiIn());
    min = std::move(_min);
  }
  catch (RtMidiError &error) {
    error.printMessage();
    exit(EXIT_FAILURE);
  }

  // RtMidiOut constructor
  try {
    std::unique_ptr<RtMidiOut> _mout(new RtMidiOut());
    mout = std::move(_mout);
  }
  catch (RtMidiError &error) {
    error.printMessage();
    exit(EXIT_FAILURE);
  }

  midiOut(mout);

  std::thread seq(sequencer, std::ref(mout));

  running.store(true);
  cv.notify_one();

  midiIn(min);

  running.store(false);

  seq.join();

  return 0;
}