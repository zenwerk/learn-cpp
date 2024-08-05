#include <iostream>
#include "RtMidi.h"

#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

void _sleep(int milliseconds) {
#ifdef _WINDOWS
  Sleep(milliseconds);
#else
  sleep(milliseconds);
#endif
}

void printAvailablePorts(std::unique_ptr<RtMidiIn> &min, std::unique_ptr<RtMidiOut> &mout) {
  // Check inputs.
  unsigned int nPorts = min->getPortCount();
  std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";
  std::string portName;
  for (unsigned int i = 0; i < nPorts; i++) {
    try {
      portName = min->getPortName(i);
    }
    catch (RtMidiError &error) {
      error.printMessage();
    }
    std::cout << "  Input Port #" << i << ": " << portName << '\n';
  }

  // Check outputs.
  nPorts = mout->getPortCount();
  std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";
  for (unsigned int i = 0; i < nPorts; i++) {
    try {
      portName = mout->getPortName(i);
    }
    catch (RtMidiError &error) {
      error.printMessage();
    }
    std::cout << "  Output Port #" << i << ": " << portName << '\n';
  }
  std::cout << '\n';
}

void midiOut(std::unique_ptr<RtMidiOut> &mout) {
  std::vector<unsigned char> message;
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
  message[0] = 144; // 0x90
  message[1] = 64;
  message[2] = 90;
  mout->sendMessage(&message);
  _sleep(500); // Platform-dependent ... see example in tests directory.
  // Note Off: 128, 64, 40
  message[0] = 128; // 0x80
  message[1] = 64;
  message[2] = 40;
  mout->sendMessage(&message);
  std::cout << "Midi OUT!" << std::endl;
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

  printAvailablePorts(min, mout);

  midiOut(mout);

  midiIn(min);

  return 0;
}

