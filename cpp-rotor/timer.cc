#include <chrono>

#include <rotor.hpp>
#include <rotor/asio.hpp>
#include <utility>

namespace asio = boost::asio;
namespace posix_time = boost::posix_time;

struct Payload{};
using TimerMessage = rotor::message_t<Payload>;

class Timer {
    using clock = std::chrono::steady_clock;
    clock::time_point start_time_ = {};
    clock::duration elapsed_time_ = {};
    rotor::address_ptr_t addr_ = {};

public:
    void SetActorAddr(const rotor::address_ptr_t &addr) { addr_ = addr; }

    [[nodiscard]] bool IsRunning() const {
        return start_time_ != clock::time_point{};
    }

    void Start() {
        if (!IsRunning()) {
            start_time_ = clock::now();
        }
    }

    void Stop() {
        if (IsRunning()) {
            elapsed_time_ += clock::now() - start_time_;
            start_time_ = {};
        }
    }

    void Reset() {
        start_time_ = {};
        elapsed_time_ = {};
    }

    clock::duration GetElapsed() {
        auto result = elapsed_time_;
        if (IsRunning()) {
            result += clock::now() - start_time_;
        }
        return result;
    }

    long long GetElapsedMilliSeconds() {
        auto elapsed = GetElapsed();
        return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    }

    long long GetElapsedSeconds() {
        auto elapsed = GetElapsed();
        return std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    }
};



struct TimerActor : public rotor::actor_base_t {
    using rotor::actor_base_t::actor_base_t;
    std::optional<rotor::request_id_t> timer_id;
    rotor::address_ptr_t timer_addr;
    int i = 0;
    Timer timer;

    void configure(rotor::plugin::plugin_base_t &plugin) noexcept override {
        rotor::actor_base_t::configure(plugin);
        plugin.with_casted<rotor::plugin::starter_plugin_t>([&](auto &p){
            p.subscribe_actor(&TimerActor::on_timer_, get_address());
        });
    }

    void on_timer_(TimerMessage&) noexcept {
        std::cout << "Elapsed " << timer.GetElapsedSeconds() << " seconds" << std::endl;
    }
    void on_timer(rotor::request_id_t, bool cancelled) noexcept {
        timer_id.reset();
        std::cout << "Elapsed " << ++i << " seconds" << std::endl;
        timer_id = start_timer(boost::posix_time::seconds{1}, *this, &TimerActor::on_timer);
    }

    void on_start() noexcept override {
        rotor::actor_base_t::on_start();

        timer_id = start_timer(boost::posix_time::seconds{1}, *this, &TimerActor::on_timer);
//        auto addr = get_address();
//        timer.SetActorAddr(addr);
//        timer.Start();
//        while (timer.GetElapsedSeconds() < 5) {
//            if (timer.GetElapsedMilliSeconds() % 1000 == 0)
//                send<Payload>(addr);
//        }
    }
};


int main() {
    asio::io_context io_ctx;

    // 1. Create SystemContext
    auto sys_ctx = rotor::asio::system_context_asio_t::ptr_t(new rotor::asio::system_context_asio_t(io_ctx));

    // 2. Create Supervisor
    auto strand = std::make_shared<asio::io_context::strand>(io_ctx);
    auto timeout = boost::posix_time::milliseconds{500};
    auto sup = sys_ctx->create_supervisor<rotor::asio::supervisor_asio_t>()
        .strand(strand)
        .timeout(timeout)
        .finish();

    auto ta = sup->create_actor<TimerActor>().timeout(timeout).finish();

    sup->do_process();
    io_ctx.run();

    sup->do_shutdown();

    return 0;
}