#include <libremidi/libremidi.hpp>

#ifdef _WIN32
#include <windows.h>
#else

#include <unistd.h>

#endif

void _sleep(int milliseconds) {
#ifdef _WIN32
  Sleep(milliseconds);
#else
  sleep(milliseconds);
#endif
}

int main() {
  // Enumerating midi input ports
  libremidi::midi_in min;
  for (int i = 0, N = min.get_port_count(); i < N; i++)
    std::cout << "MIDI IN: " << min.get_port_name(i) << std::endl;

  // open default midi input port and set callback;
  min.open_port();
  min.set_callback([](const libremidi::message &msg) {
    std::cout << "------------------------" << std::endl;
    std::cout << "Input message size: " << msg.size() << std::endl;
    for (int i = 0; i < msg.size(); i++)
      std::cout << "Byte " << i << " = " << msg[i] << std::endl;
    std::cout << "stamp = " << msg.timestamp << std::endl;
  });

  // Enumerating midi output ports
  libremidi::midi_out mout;
  for (int i = 0, N = mout.get_port_count(); i < N; i++)
    std::cout << "MIDI OUT: " << mout.get_port_name(i) << std::endl;

  // open default midi output port
  mout.open_port();
  std::vector<unsigned char> message;
  // Program change: 192, 5
  message.push_back(192);
  message.push_back(5);
  mout.send_message(message);
  // Control Change: 176, 7, 100 (volume)
  message[0] = 176;
  message[1] = 7;
  message.push_back(100);
  mout.send_message(message);
  // Note On: 144, 64, 90
  message[0] = 144;
  message[1] = 64;
  message[2] = 90;
  mout.send_message(message);
  _sleep(1000); // Platform-dependent ... see example in tests directory.
  // Note Off: 128, 64, 40
  message[0] = 128;
  message[1] = 64;
  message[2] = 40;
  mout.send_message(message);
  std::cout << "Midi OUT!" << std::endl;

  std::cout << "\nReading MIDI input ... press <enter> to quit.\n";
  char input;
  std::cin.get(input);

  return 0;
}