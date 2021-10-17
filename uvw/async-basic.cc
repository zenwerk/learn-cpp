#include <iostream>
#include <thread>

#include <uvw.hpp>


int main() {
    int size = 1024;
    auto loop = uvw::Loop::getDefault();
    auto async = loop->resource<uvw::AsyncHandle>();

    async->on<uvw::ErrorEvent>([](auto &&...) {
        std::cout << "Error event!" << std::endl;
    });
    async->on<uvw::AsyncEvent>([](const uvw::AsyncEvent &, uvw::AsyncHandle &handle) {
        auto percentage = handle.data<double>();
        std::cerr << "Downloaded " << *percentage << "%" << std::endl;
    });

    if (async->init())
        std::cout << "Async init" << std::endl;

    auto req = loop->resource<uvw::WorkReq>([&async, &size]() {
        int downloaded = 0;
        while (downloaded < size) {
            auto percentage = std::make_shared<double>(downloaded * 100.0 / size);
            async->data(percentage);
            async->send();

            std::this_thread::sleep_for(std::chrono::seconds(1));
            downloaded += (200 + rand()) % 1000; // can only download max 1000bytes/sec,
            // but at least a 200;
        }
    });
    req->on<uvw::WorkEvent>([&async](auto &...) {
        std::cerr << "Download complete" << std::endl;
        async->close();
    });

    req->queue();

    loop->run();
}