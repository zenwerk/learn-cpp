#include <iostream>
#include <uvw.hpp>

int main() {
    int64_t counter = 0;
    auto loop = uvw::Loop::getDefault();

    auto idler = loop->resource<uvw::IdleHandle>();

    idler->on<uvw::IdleEvent>([&counter](const uvw::IdleEvent &, uvw::IdleHandle &idler){
        counter++;

        if (counter >= (int64_t)30e5)
            idler.stop();
        if (counter % (uint64_t)10e5 == 0)
            std::cout << counter << std::endl;
    });

    if (idler->init())
        std::cout << "Init IdleHandler" << std::endl;
    idler->start();

    std::cout << "Idling..." << std::endl;
    loop->run();
    std::cout << "Exit" << std::endl;

    return 0;
}