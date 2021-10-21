#include <iostream>
#include <uvw.hpp>


int main() {
    int count= 0;
    auto loop = uvw::Loop::getDefault();
    auto timer = loop->resource<uvw::TimerHandle>();

    if (timer->init())
        std::cout << "Timer init" << std::endl;

    timer->on<uvw::TimerEvent>([&count](const uvw::TimerEvent &ev, uvw::TimerHandle &handle){
        count++;
        std::cout << "Timer! " << handle.dueIn().count() << std::endl;
        if (count > 5)
            handle.close();
    });

    timer->start(std::chrono::seconds(0), std::chrono::seconds(1));

    loop->run();
}