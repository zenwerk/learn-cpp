#include <iostream>
#include <thread>
#include <uvw.hpp>


#define FIB_UNTIL 10

long fib_(long t) {
    if (t == 0 || t == 1)
        return 1;
    else
        return fib_(t - 1) + fib_(t - 2);
}

void fib(uv_work_t *req) {
    int n = *(int *) req->data;
    if (rand() % 2)
        std::this_thread::sleep_for(std::chrono::seconds(1));
    else
        std::this_thread::sleep_for(std::chrono::seconds(3));
    long fib = fib_(n);
    fprintf(stderr, "%dth fibonacci is %lu\n", n, fib);
}

void after_fib(uv_work_t *req, int status) {
    fprintf(stderr, "Done calculating %dth fibonacci\n", *(int *) req->data);
}

int main() {
    int data[FIB_UNTIL];
    uv_work_t req[FIB_UNTIL];

    auto loop = uvw::Loop::getDefault();
    auto oneShot = loop->resource<uvw::WorkReq>([]() {
        // uv_work_t や handle のようなものが受け渡せない
        std::cout << std::endl;
        std::cout << "uvw default task request!";
        std::cout << std::endl;
    });
    oneShot->on<uvw::WorkEvent>([](const uvw::WorkEvent &, auto &){
        std::cout << "Request after callback;" << std::endl;
    });

    for (int i = 0; i < FIB_UNTIL; i++) {
        data[i] = i;
        req[i].data = (void *) &data[i];
        uv_queue_work(loop->raw(), &req[i], fib, after_fib);
    }

    oneShot->queue();

    loop->run();
}