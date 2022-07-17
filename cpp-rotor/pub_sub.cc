#include <rotor.hpp>
#include <rotor/asio.hpp>

namespace asio = boost::asio;
namespace posix_time = boost::posix_time;

struct Payload {};
using SampleMessage = rotor::message_t<Payload>;

struct Publisher : public rotor::actor_base_t {
    using rotor::actor_base_t::actor_base_t;
    rotor::address_ptr_t pub_addr;

    void set_pub_addr(const rotor::address_ptr_t &addr) { pub_addr = addr; }

    void on_start() noexcept override {
        rotor::actor_base_t::on_start();
        send<Payload>(pub_addr);
    }
};

struct Subscriber : public rotor::actor_base_t {
    using rotor::actor_base_t::actor_base_t;
    rotor::address_ptr_t pub_addr;

    void set_pub_addr(const rotor::address_ptr_t &addr) { pub_addr = addr; }

    void configure(rotor::plugin::plugin_base_t &plugin) noexcept override {
        rotor::actor_base_t::configure(plugin);
        plugin.with_casted<rotor::plugin::starter_plugin_t>([&](auto &p) {
            p.subscribe_actor(&Subscriber::on_payload, pub_addr);
        });
    }

    void on_payload(SampleMessage&) noexcept {
        std::cout <<"Received message on " << static_cast<void *>(this) << std::endl;
    }
};

int main() {
    asio::io_context io_ctx;

    // 1. Create SystemContext
    auto sysctx = rotor::asio::system_context_asio_t::ptr_t (new rotor::asio::system_context_asio_t(io_ctx));

    // 2. Create Supervisor
    auto strand = std::make_shared<asio::io_context::strand>(io_ctx);
    auto timeout = boost::posix_time::milliseconds{500};
    auto sup = sysctx->create_supervisor<rotor::asio::supervisor_asio_t>()
        .strand(strand)
        .timeout(timeout)
        .finish();

    // 3. Create Actors
    auto pub_addr = sup->create_address();
    auto pub = sup->create_actor<Publisher>().timeout(timeout).finish();
    auto sub1 = sup->create_actor<Subscriber>().timeout(timeout).finish();
    auto sub2 = sup->create_actor<Subscriber>().timeout(timeout).finish();

    // 4. Set address
    pub->set_pub_addr(pub_addr);
    sub1->set_pub_addr(pub_addr);
    sub2->set_pub_addr(pub_addr);

    // 5. Do process
    sup->do_process();
    io_ctx.run();

    // 6. Shutdown
    sup->do_shutdown();

    return 0;
}