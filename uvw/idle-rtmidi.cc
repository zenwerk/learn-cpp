#include <iostream>

#include <RtMidi.h>
#include <uvw.hpp>

#include <Windows.h>


void mycallback(double deltatime, std::vector<unsigned char> *message, void *userData) {
    auto async = static_cast<uvw::AsyncHandle *>(userData);

//    async->data(std::shared_ptr<unsigned char>(message->data()));
//    async->send();
}

void midiOut(std::shared_ptr<RtMidiOut> &mout) {
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
    // _sleep(500); // Platform-dependent ... see example in tests directory.
    Sleep(500);
    // Note Off: 128, 64, 40
    message[0] = 128;
    message[1] = 64;
    message[2] = 40;
    mout->sendMessage(&message);
    std::cout << "Midi OUT!" << std::endl;
}


int main() {
    int64_t counter = 0;
    auto loop = uvw::Loop::getDefault();
    auto idler = loop->resource<uvw::IdleHandle>();
    auto async = loop->resource<uvw::AsyncHandle>();

    // MIDI Output
    auto midiout = std::make_shared<RtMidiOut>();
    // Check available ports.
    if (midiout->getPortCount() == 0) {
        std::cout << "No ports available!\n";
        return 0;
    }
    // Open first available port.
    try {
        midiout->openPort(0);
    }
    catch (RtMidiError &error) {
        error.printMessage();
        return 0;
    }

    // MIDI Input
    auto midiin = std::make_shared<RtMidiIn>();
    unsigned int nPorts = midiin->getPortCount();
    if (nPorts == 0) {
        std::cout << "No ports available!\n";
        return 0;
    }
    midiin->openPort(0);
    midiin->setCallback(&mycallback, &async);
    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes(false, false, false);

    // IdleHandle
    idler->data(midiout);
    idler->on<uvw::IdleEvent>([&counter](const uvw::IdleEvent &, uvw::IdleHandle &idler) {
        counter++;

        if (counter >= (int64_t) 30e5)
            idler.stop();
        if (counter % (uint64_t) 10e5 == 0) {
            auto mout = idler.data<RtMidiOut>();
            midiOut(mout);
        }
    });

    if (idler->init())
        std::cout << "Init IdleHandler" << std::endl;
    idler->start();

    // AsyncHandle
    async->on<uvw::AsyncEvent>([](const uvw::AsyncEvent &ev, uvw::AsyncHandle &async) {
        std::cout << "MIDI IN" << std::endl;
//        auto message = async.data<std::vector<unsigned char>>();
//
//        unsigned int nBytes = message->size();
//        for (unsigned int i = 0; i < nBytes; i++)
//            std::cout << "Byte " << i << " = " << (int) message->at(i) << ", ";
        //if (nBytes > 0)
        //    std::cout << "stamp = " << data->deltatime << std::endl;
    });

    if (async->init())
        std::cout << "Init AsyncHandler" << std::endl;

    std::cout << "Idling..." << std::endl;
    loop->run();
    std::cout << "Exit" << std::endl;

    return 0;
}
