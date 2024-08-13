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

using TimeUnit = std::chrono::microseconds;

template<typename T>
constexpr uint32_t get_unit_value();

template<>
constexpr uint32_t get_unit_value<std::chrono::milliseconds>() {
    return 1000;
}

template<>
constexpr uint32_t get_unit_value<std::chrono::microseconds>() {
    return 1000000;
}

template<>
constexpr uint32_t get_unit_value<std::chrono::nanoseconds>() {
    return 1000000000;
}

void calc_tick() {
  if (tpqn <= 0 || tpqn % midi_clock_base != 0) {
    std::cerr << "Invalid TPQN value: " << tpqn << std::endl;
    return;
  }

  auto base = get_unit_value<TimeUnit>();

  qn_time = (60.0f / bpm) * base; // BPMから四分音符(1拍)の時間を計算 (micros)
  tick_time = qn_time / tpqn; // 四分音符の時間を分解能(TPQN)で割って 1 tick に必要な時間を計算 (microsec)
  f8_interval = tpqn / midi_clock_base; // 四分音符の分解能から MIDI Clock イベントの送信間隔を計算
}


void sequencer(std::unique_ptr<RtMidiOut> &mout) {
  uint32_t tick_count = 0;
  uint32_t f8_count = 0;

  // Clock
  using clock = std::chrono::steady_clock;
  clock::time_point start_time = {};
  clock::duration elapsed_time = {};
  uint32_t count = 0;

  bool note_on = false;
  start_time = clock::now();

  // Open first available port.
  unsigned int nPorts = mout->getPortCount();
  if (nPorts == 0) {
    std::cout << "No ports available!\n";
    return;
  }
  try {
    mout->openPort(0);
  }
  catch (RtMidiError &error) {
    error.printMessage();
    return;
  }

  while (count < 6) {
    elapsed_time = clock::now() - start_time;
    auto elapsed_microsec = std::chrono::duration_cast<TimeUnit>(elapsed_time).count();

    if (elapsed_microsec >= tick_time) {
      // 1 tick 経過
      tick_count++;
      std::cout << " tick: " << tick_count << "   elapesd: " << elapsed_microsec << std::endl;

      if (tick_count % f8_interval == 0) {
        // MIDI Clock イベントを送信
        f8_count++;
        auto f8 = std::vector<uint8_t>{0xF8};
        mout->sendMessage(&f8);
        std::cout << "F8: " << f8_count << std::endl;
      }

      if (tick_count % tpqn == 0) {
        count++;
        // 1拍経過
        f8_count = 0;

        // 1拍ごとに ON/OFF を切り替える
        if (note_on) {
          // ここで Node OFF を送信する
          note_on = false;
          auto msg = std::vector<uint8_t>{0x80, 63, 40};
          mout->sendMessage(&msg);
          std::cout << "OFF: " << elapsed_microsec << "μs " << count <<  std::endl;
        } else {
          // ここで Node ON を送信する
          note_on = true;
          auto msg = std::vector<uint8_t>{0x90, 63, 90};
          mout->sendMessage(&msg);
          std::cout << "ON:  " << elapsed_microsec << "μs " << count << std::endl;
        }
      }

      // 1 tick に必要な時間だけ経過したのでリセット
      start_time = clock::now();
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1));
  }

  auto msg = std::vector<unsigned char>{0x80, 63, 90};
  mout->sendMessage(&msg);
}


void seq2(std::unique_ptr<RtMidiOut> &mout) {
  uint32_t count = 0;

  // Clock
  using clock = std::chrono::steady_clock;
  clock::time_point start_time = {};
  clock::duration elapsed_time = {};

  bool note_on = false;
  start_time = clock::now();

  while (count < 10) {
    elapsed_time = clock::now() - start_time;
    auto elapsed_microsec = std::chrono::duration_cast<TimeUnit>(elapsed_time).count();

    if (elapsed_microsec >= qn_time) {
      count++;

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

      // 1 qn に必要な時間だけ経過したのでリセット
      start_time = clock::now();
    }

    std::this_thread::sleep_for(std::chrono::nanoseconds(100));
  }
}


int main() {
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

  calc_tick();
  std::cout << "Start sequencer" << std::endl;
  std::cout << "TPQN: " << tpqn << std::endl;
  std::cout << "BPM: " << bpm << std::endl;
  std::cout << "QN Time: " << qn_time << " microsec" << std::endl;
  std::cout << "Tick Time: " << tick_time << " microsec" << std::endl;
  std::cout << "F8 Interval: " << f8_interval << std::endl;
  std::cout << "-------------------" << std::endl;

  sequencer(mout);
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  std::cout << "-------------------" << std::endl;
  seq2(mout);

  return 0;
}