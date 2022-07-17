//
// Copyright (c) 2019-2021 Ivan Baidakou (basiliscos) (the dot dmol at gmail dot com)
//
// Distributed under the MIT Software License
//

#include "rotor.hpp"
#include "rotor/asio.hpp"
#include <boost/asio.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <random>
#include <optional>
#include <unordered_map>

namespace asio = boost::asio;
namespace pt = boost::posix_time;
namespace ra = rotor::asio;

/**
 * I/Oの低信頼性をシミュレートするための定数
 */
namespace constants {
    static float failure_probability = 0.70f; // 応答失敗する確率
    static pt::time_duration ping_timeout = pt::milliseconds{100};
    static pt::time_duration ping_reply_base = pt::milliseconds{50};
    static pt::time_duration check_interval = pt::milliseconds{3000};
    static std::uint32_t ping_reply_scale = 70;
} // namespace constants

/**
 * なんらかの制御で使っているリソースID
 */
namespace resource {
    rotor::plugin::resource_id_t timer = 0;
    rotor::plugin::resource_id_t ping = 1;
} // namespace resource

/**
 * ペイロード用構造体
 */
namespace payload {
    struct pong_t {};
    struct ping_t {
        using response_t = pong_t;
    };
} // namespace payload

/**
 * 実際にアクター間でやりとりされるメッセージ
 */
namespace message {
    using ping_t = rotor::request_traits_t<payload::ping_t>::request::message_t;
    using pong_t = rotor::request_traits_t<payload::ping_t>::response::message_t;
    using cancel_t = rotor::request_traits_t<payload::ping_t>::cancel::message_t;
} // namespace message

/**
 * pinger はクライアントとして振る舞う
 */
struct pinger_t : public rotor::actor_base_t {

    using rotor::actor_base_t::actor_base_t;

    std::optional<rotor::request_id_t> timer_id;
    std::optional<rotor::request_id_t> request_id;
    std::uint32_t attempts = 0;
    rotor::address_ptr_t ponger_addr;

    /**
     * アクター初期化時に呼ばれる設定関数
     */
    void configure(rotor::plugin::plugin_base_t &plugin) noexcept override {
        rotor::actor_base_t::configure(plugin);

        /* on_pong で他のアクターからのメッセージを待ち受ける */
        plugin.with_casted<rotor::plugin::starter_plugin_t>([](auto &p) {
            p.subscribe_actor(&pinger_t::on_pong);
        });

        // 送信先をレジストリから名前"ponger" で検索する
        plugin.with_casted<rotor::plugin::registry_plugin_t>([&](auto &p) {
            p.discover_name("ponger", ponger_addr, true).link();
        });
    }

    /**
     * アクター開始時に呼ばれるライフサイクル関数
     */
    void on_start() noexcept override {
        rotor::actor_base_t::on_start();

        do_ping(); // ping 送信

        /** Step1
         * タイムアウト時に自己停止するタイマーを設定する
         * timer_id にID記録, このIDはタイマーのキャンセルやshutdownに必要になる
         */
        timer_id = start_timer(constants::check_interval, *this, &pinger_t::on_custom_timeout);

        /** Step2
         * タイマー`リソース`を取得する -> shutdown/init をLockさせる目的のために取得する
         *   リソース取得処理がないと, タイマーが actor の shutdown 後に発火するため.
         * on_custom_timeout タイマハンドラでは逆に`リソース`を解放し, shutdown 処理をLockさせないようにする.
         *   timer_id も reset しないといけん
         * タイマーハンドラは、キャンセルされようが起動されようが、いずれにせよ一度は起動されることが保証される
         *   ただし、今回の実装(on_custom_timeout)では, タイマーがトリガーされた場合 pinger は自身を shutdown(do_shutdown()) する必要がある.
         */
        resources->acquire(resource::timer);
    }

    /** 非同期操作（リクエスト）を開始 (do_pingの呼び出し)
     * リソースを獲得して記録(resources->acquire),
     * レスポンスがあればリソースを解放してリクエストを忘れるという同じパターン
     */
    void do_ping() noexcept {
        resources->acquire(resource::ping); /* pingリソースを確保する, 解放されるまでinit/shutdownはLockされる */
        request_id = request<payload::ping_t>(ponger_addr).send(constants::ping_timeout);
        ++attempts;
    }

    /** do_ping のタイムアウト処理
     * on_start で設定された start_timer が発火すると呼ばれる
     */
    void on_custom_timeout(rotor::request_id_t, bool cancelled) {
        resources->release(resource::timer); // 確保していたタイマーリソース解放する
        timer_id.reset();
        std::cout << "pinger_t, (" << (void *)this << "), on_custom_timeout, cancelled: " << cancelled << "\n";
        if (!cancelled) {
            // cancel_timer されていなければ、タイムアウト(処理が失敗した)としてアクターをshutdownする
            do_shutdown();
        }
    }

    // pinger の shutdown 処理
    void shutdown_start() noexcept override {
        std::cout << "pinger_t, (" << (void *)this << ") shutdown_start() \n";
        // pending request があったらキャンセル
        if (request_id)
            send<message::cancel_t>(ponger_addr, get_address());
        // active timer があったらキャンセル
        if (timer_id) {
            cancel_timer(*timer_id);
            timer_id.reset();
        }
        // shutdown 処理を続行する
        rotor::actor_base_t::shutdown_start();
    }

    void shutdown_finish() noexcept override {
        std::cout << "pinger_t, (" << (void *)this << ") finished attempts done " << attempts << "\n";
        rotor::actor_base_t::shutdown_finish();
    }

    void on_pong(message::pong_t &msg) noexcept {
        resources->release(resource::ping); // レスポンスがあればpingリソースを解放
        request_id.reset();                 // リクエストを忘れる
        auto &ec = msg.payload.ee;
        if (!ec) {
            std::cout << "pinger_t, (" << (void *)this << ") success!, pong received, attemps : " << attempts << "\n";
            do_shutdown(); // ちゃんと pong が返ってきたら shutdown
        } else {
            std::cout << "pinger_t, (" << (void *)this << ") pong failed (" << attempts << ")\n";
            if (timer_id) { // つまり、まだアクターが動作している(タイムアウトしていない)
                do_ping();  // retryする
            }
        }
    }
};

/**
 * ponger はサーバーとして振る舞う
 * つまり他のアクターからのメッセージを待ち受けるアクター
 */
struct ponger_t : public rotor::actor_base_t {
    using generator_t = std::mt19937;
    using distrbution_t = std::uniform_real_distribution<double>;

    // アクターがサーバーの役割を果たすので、通常は `on_start()` メソッドを持たない

    // ponger は Pingリクエストに対してすぐにresponseしないので、次のresponseのために内部で保存しておく必要がある
    using message_ptr_t = rotor::intrusive_ptr_t<message::ping_t>;
    // (非同期のため)遅延した ping 応答のためのタイマーを表す
    // つまり、Pingリクエストが到着すると、タイマーが生成され、元のリクエストへのスマートポインタとともに保存される
    using requests_map_t = std::unordered_map<rotor::request_id_t, message_ptr_t>;

    std::random_device rd;
    generator_t gen;
    distrbution_t dist;
    requests_map_t requests;

    explicit ponger_t(config_t &cfg) : rotor::actor_base_t(cfg), gen(rd()) {}

    void configure(rotor::plugin::plugin_base_t &plugin) noexcept override {
        rotor::actor_base_t::configure(plugin);
        plugin.with_casted<rotor::plugin::starter_plugin_t>([](auto &p) {
            p.subscribe_actor(&ponger_t::on_ping);
            p.subscribe_actor(&ponger_t::on_cancel);
        });

        // サーバーとして発見可能になるため、registry に "ponger" として自信を登録する
        plugin.with_casted<rotor::plugin::registry_plugin_t>([&](auto &p) {
            p.register_name("ponger", get_address());
        });
    }

    // 非同期操作の常として、タイマーリソースを取得する
    // しかし、アクターがshutdownしているときに非同期処理（タイマー）を開始すると、アクターがすぐにエラーで応答してしまうので、アクターの状態を追加でチェックする必要がある
    void on_ping(message::ping_t &req) noexcept {
        // 状態がOPERATIONALでなければエラーを返す
        if (state != rotor::state_t::OPERATIONAL) {
            auto ec = rotor::make_error_code(rotor::error_code_t::cancelled);
            reply_with_error(req, make_error(ec));
            return;
        }

        // サイコロを振って遅延時間をシミュレーションする
        auto dice = constants::ping_reply_scale * dist(gen);
        pt::time_duration reply_after = constants::ping_reply_base + pt::millisec{(int)dice};

        // タイマーを取得
        // 乱数から得たreply_after時間後に on_ping_timer コールバックが発火して pong を返す、という流れ
        auto timer_id = start_timer(reply_after, *this, &ponger_t::on_ping_timer);
        // タイマーリソースを取得
        resources->acquire(resource::timer);
        // タイマIDと対応したpingメッセージをmapにして保存しておく
        requests.emplace(timer_id, message_ptr_t(&req));
    }

    // pinger からのキャンセルメッセージを受信した
    void on_cancel(message::cancel_t &notify) noexcept {
        auto request_id = notify.payload.id;
        auto &source = notify.payload.source;
        std::cout << "cancellation notify\n";
        auto predicate = [&](auto &it) {
            return it.second->payload.id == request_id && it.second->payload.origin == source;
        };
        auto it = std::find_if(requests.begin(), requests.end(), predicate);
        if (it != requests.end()) {
            cancel_timer(it->first);
        }
    }

    // タイマーハンドラの実装
    void on_ping_timer(rotor::request_id_t timer_id, bool cancelled) noexcept {
        resources->release(resource::timer); // タイマーリソースを解放
        auto it = requests.find(timer_id);

        if (!cancelled) {
            // タイマーがキャンセルされていなければ、つまり cancel_timer が呼ばれていなければ成功
            auto dice = dist(gen);
            // 一定確率でレスポンスが失敗するようにエミュレートしている
            // ここで失敗した場合というのはネットワークのどこかでリクエストが消えてしまった場合を表現している
            if (dice > constants::failure_probability) {
                auto &msg = it->second;
                reply_to(*msg);
            }
        } else {
            // キャンセルされていたら対応するエラーコードを返す
            // 普通キャンセルshutdown処理中に起きる. そのときはASAPで終了させることが望ましい, そうしないと shutdown
            // timer が起動されデフォルトでは std::terminate が呼ばれる これは、shutdown timeout
            // を調整することで回避できます. つまり、shutdown timeout
            // を要求タイムアウトより大きくすれば良いが、かなり不安定な状況であり、お勧めしません。
            auto ec = rotor::make_error_code(rotor::error_code_t::cancelled);
            reply_with_error(*it->second, make_error(ec));
        }

        requests.erase(it); // リクエスト保持mapから削除する
    }

    void shutdown_start() noexcept override {
        // ペンディング中のpingレスポンスを全削除する
        // レスポンスは、キャンセルコールバックを呼び出す際に削除される
        while (!requests.empty()) {
            auto &timer_id = requests.begin()->first;
            cancel_timer(timer_id);
        }
        rotor::actor_base_t::shutdown_start();
    }

    void shutdown_finish() noexcept override {
        std::cout << "ponger_t, shutdown_finish\n";
        rotor::actor_base_t::shutdown_finish();
    }
};

struct custom_supervisor_t : ra::supervisor_asio_t {
    using ra::supervisor_asio_t::supervisor_asio_t;

    void on_child_shutdown(actor_base_t *) noexcept override {
        // pinger が shutdown したら自身もshutdown
        if (state < rotor::state_t::SHUTTING_DOWN) {
            do_shutdown();
        }
    }

    void shutdown_finish() noexcept override {
        ra::supervisor_asio_t::shutdown_finish(); // asio も止める
        strand->context().stop();
    }
};

std::atomic_bool shutdown_flag = false;

int main() {
    asio::io_context io_context;
    auto system_context = rotor::asio::system_context_asio_t::ptr_t{new rotor::asio::system_context_asio_t(io_context)};
    auto strand = std::make_shared<asio::io_context::strand>(io_context);
    auto timeout = pt::milliseconds{50};
    auto sup = system_context->create_supervisor<custom_supervisor_t>()
        .strand(strand)
        .create_registry() // 仲介者(つまりここがレジストリ)
        .timeout(timeout)
        .guard_context(false)
        .finish();

    // sup->create_actor<pinger_t>().timeout(timeout).finish();
    sup->create_actor<pinger_t>().timeout(timeout).finish();
    sup->create_actor<ponger_t>().timeout(timeout).finish();

    sup->start();
#ifndef _WIN32
    struct sigaction act;
    act.sa_handler = [](int) { shutdown_flag = true; };
    if (sigaction(SIGINT, &act, nullptr) != 0) {
        std::cout << "critical :: cannot set signal handler\n";
        return -1;
    }
    auto console_thread = std::thread([&] {
        while (!shutdown_flag) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        sup->shutdown();
    });
#endif

    io_context.run();

#ifndef _WIN32
    shutdown_flag = true;
    console_thread.join();
#endif
    return 0;
}

/* output samples:

(all ping failed)
./examples/boost-asio/ping-pong-timer
pinger_t, (0x556d13bbd8a0) pong failed (1)
pinger_t, (0x556d13bbd8a0) pong failed (2)
pinger_t, (0x556d13bbd8a0) pong failed (3)
pinger_t, (0x556d13bbd8a0) pong failed (4)
pinger_t, (0x556d13bbd8a0) pong failed (5)
pinger_t, (0x556d13bbd8a0) pong failed (6)
pinger_t, (0x556d13bbd8a0) pong failed (7)
pinger_t, (0x556d13bbd8a0) pong failed (8)
pinger_t, (0x556d13bbd8a0) pong failed (9)
pinger_t, (0x556d13bbd8a0) pong failed (10)
pinger_t, (0x556d13bbd8a0) pong failed (11)
pinger_t, (0x556d13bbd8a0) pong failed (12)
pinger_t, (0x556d13bbd8a0) pong failed (13)
pinger_t, (0x556d13bbd8a0) pong failed (14)
pinger_t, (0x556d13bbd8a0) pong failed (15)
pinger_t, (0x556d13bbd8a0) pong failed (16)
pinger_t, (0x556d13bbd8a0) pong failed (17)
pinger_t, (0x556d13bbd8a0) pong failed (18)
pinger_t, (0x556d13bbd8a0) pong failed (19)
pinger_t, (0x556d13bbd8a0) pong failed (20)
pinger_t, (0x556d13bbd8a0) pong failed (21)
pinger_t, (0x556d13bbd8a0) pong failed (22)
pinger_t, (0x556d13bbd8a0) pong failed (23)
pinger_t, (0x556d13bbd8a0) pong failed (24)
pinger_t, (0x556d13bbd8a0) pong failed (25)
pinger_t, (0x556d13bbd8a0) pong failed (26)
pinger_t, (0x556d13bbd8a0) pong failed (27)
pinger_t, (0x556d13bbd8a0) pong failed (28)
pinger_t, (0x556d13bbd8a0) pong failed (29)
pinger_t, (0x556d13bbd8a0), on_custom_timeout, cancelled: 0
pinger_t, (0x556d13bbd8a0) shutdown_start()
pinger_t, (0x556d13bbd8a0) pong failed (30)
pinger_t, (0x556d13bbd8a0) finished attempts done 30
ponger_t, shutdown_finish

(11-th ping was successful)
./examples/boost-asio/ping-pong-timer
pinger_t, (0x55f9f90048a0) pong failed (1)
pinger_t, (0x55f9f90048a0) pong failed (2)
pinger_t, (0x55f9f90048a0) pong failed (3)
pinger_t, (0x55f9f90048a0) pong failed (4)
pinger_t, (0x55f9f90048a0) pong failed (5)
pinger_t, (0x55f9f90048a0) pong failed (6)
pinger_t, (0x55f9f90048a0) pong failed (7)
pinger_t, (0x55f9f90048a0) pong failed (8)
pinger_t, (0x55f9f90048a0) pong failed (9)
pinger_t, (0x55f9f90048a0) pong failed (10)
pinger_t, (0x55f9f90048a0) success!, pong received, attemps : 11
pinger_t, (0x55f9f90048a0) shutdown_start()
pinger_t, (0x55f9f90048a0), on_custom_timeout, cancelled: 1
pinger_t, (0x55f9f90048a0) finished attempts done 11
ponger_t, shutdown_finish

(premature termination via CTRL+C pressing)
./examples/boost-asio/ping-pong-timer
pinger_t, (0x55d5d95d98a0) pong failed (1)
pinger_t, (0x55d5d95d98a0) pong failed (2)
pinger_t, (0x55d5d95d98a0) pong failed (3)
pinger_t, (0x55d5d95d98a0) pong failed (4)
pinger_t, (0x55d5d95d98a0) pong failed (5)
pinger_t, (0x55d5d95d98a0) pong failed (6)
^Cpinger_t, (0x55d5d95d98a0) shutdown_start()
pinger_t, (0x55d5d95d98a0), on_custom_timeout, cancelled: 1
pinger_t, (0x55d5d95d98a0) pong failed (7)
pinger_t, (0x55d5d95d98a0) finished attempts done 7
ponger_t, shutdown_finish


*/
