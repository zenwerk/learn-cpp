#include <iostream>
#include "RtMidi.h"

#ifdef _WINDOWS
#include <windows.h>
#else
#include <unistd.h>
#endif

//#include <uv.h>

void _sleep(int milliseconds) {
#ifdef _WINDOWS
    Sleep(milliseconds);
#else
    sleep(miliseconds);
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

void midiOut(std::unique_ptr<RtMidiOut>& mout) {
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

    return 0;
}
