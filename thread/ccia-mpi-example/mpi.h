#ifndef MPI_H
#define MPI_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

namespace messaging {


// Message の基底クラス
struct MessageBase {
  virtual ~MessageBase() = default;
};

// 具体的な個別メッセージ
template<typename Msg>
struct WrappedMessage : MessageBase {
  Msg contents;

  explicit WrappedMessage(Msg const &contents_) : contents(contents_) {}
};

// Messageキュー
// 条件変数で pop するときに empty かどうかだけチェックしているスレッドセーフ実装
class Queue {
  std::mutex m;
  std::condition_variable c;
  std::queue<std::shared_ptr<MessageBase> > q;  // 標準queue

public:
  template<typename T>
  void push(T const &msg) {
    std::lock_guard<std::mutex> lk(m);
    q.push(std::make_shared<WrappedMessage<T>>(msg));
    c.notify_all();  // 全体通知
  }

  std::shared_ptr<MessageBase> wait_and_pop() {
    std::unique_lock<std::mutex> lk(m);
    c.wait(lk, [&] { return !q.empty(); });  // empty でなければ先に進む
    auto res = q.front();
    q.pop();
    return res;
  }
};

class CloseQueue : std::exception {
};

// メソッドチェーンみたいな感じのをジェネリックにやる実装
template<typename PreviousDispatcher, typename Msg, typename Func>
class TemplateDispatcher {
  Queue *q;
  PreviousDispatcher *prev; // チェーンしてきたやつ
  Func f;
  bool chained{false};

  // 別の型の組み合わせの型でもfriendしてアクセスできるようにする
  template<typename Dispatcher, typename OtherMsg, typename OtherFunc>
  friend
  class TemplateDispatcher;

  // Queue からメッセージを待って実行
  void wait_and_dispatch() {
    for (;;) {
      auto msg = q->wait_and_pop();  // Queueからメッセージを受信
      if (dispatch(msg))  // メッセージの実行が成功するまで dispatch し続ける
        break;
    }
  }

  // メッセージを処理する
  bool dispatch(std::shared_ptr<MessageBase> const &msg) {
    if (auto *wrapper = dynamic_cast<WrappedMessage<Msg> *>(msg.get())) {
      // dynamic_castが成功した場合 -> template で指定したメッセージ型なのかどうかを判断している
      f(wrapper->contents);  // templateで渡された関数を実行
      return true;
    } else {
      // dynamic_castが失敗した場合
      return prev->dispatch(msg);  // チェーンしてきたやつのに実行を移譲する
    }
  }

public:
  TemplateDispatcher(TemplateDispatcher const &) = delete; // 参照コンストラクタは削除
  TemplateDispatcher &operator=(TemplateDispatcher const &) = delete; // 代入演算子は削除

  // moveコンストラクタ
  TemplateDispatcher(TemplateDispatcher &&other) noexcept: q(other.q), prev(other.prev), f(std::move(other.f)),
                                                           chained(other.chained) {
    other.chained = true;
  }

  // handle(OtherFunc &&of) で返される TemplateDispatcher で使われるコンストラクタ
  TemplateDispatcher(Queue *q_, PreviousDispatcher *prev_, Func &&f_) : q(q_), prev(prev_), f(std::forward<Func>(f_)) {
    prev_->chained = true;
  }

  template<typename OtherMsg, typename OtherFunc>
  TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc> handle(OtherFunc &&of) {
    // TemplateDispatcher(this.q, this, 引数で渡された関数of) を返す
    // PreviousDispatcher -> TemplateDispatcher(this), OtherMsg -> handle<...>の部分, OtherFunc -> handleの引数
    return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(q, this, std::forward<OtherFunc>(of));
  }

  ~TemplateDispatcher() noexcept(false) {
    if (!chained)
      wait_and_dispatch();
  }
};

// メッセージ処理の起点
class Dispatcher {
  Queue *q;
  bool chained{false};

  template<typename Dispatcher, typename Msg, typename Func>
  friend
  class TemplateDispatcher;

  [[noreturn]] void wait_and_dispatch() {
    for (;;) {
      auto msg = q->wait_and_pop();
      dispatch(msg);
    }
  }

  bool dispatch(std::shared_ptr<MessageBase> const &msg) noexcept(false) {
    if (dynamic_cast<WrappedMessage<CloseQueue> *>(msg.get())) {
      // dynamic_cast できなかったら CloseQueue なら例外を投げる
      // prev->dispatch の終点なので処理できないメッセージを受け取った
      throw CloseQueue();
    }
    return false;
  }

public:
  Dispatcher(Dispatcher const &) = delete;

  Dispatcher &operator=(Dispatcher const &) = delete; // コピー演算子は削除

  // moveコンストラクタ
  Dispatcher(Dispatcher &&other) noexcept: q(other.q), chained(other.chained) {
    other.chained = true;
  }

  explicit Dispatcher(Queue *q_) : q(q_) {}

  // handleはTemplateDispatcherを返す
  template<typename Message, typename Func>
  TemplateDispatcher<Dispatcher, Message, Func> handle(Func &&f) {
    return TemplateDispatcher<Dispatcher, Message, Func>(q, this, std::forward<Func>(f));
  }

  ~Dispatcher() noexcept(false) {
    if (!chained) {
      wait_and_dispatch();
    }
  }
};

// メッセージ送信者
class Sender {
  Queue *q;

public:
  Sender() : q(nullptr) {}

  explicit Sender(Queue *q_) : q(q_) {}

  // Queue にメッセージを詰める
  template<typename Message>
  void send(Message const &msg) {
    if (q)
      q->push(msg);
  }
};

// メッセージ受信者
// Receiver のインスタンス毎にスレッドセーフなQueueが作られる
class Receiver {
  Queue q;

public:
  // Receiver の Queue への参照保持する Senderを生成して返す
  operator Sender() {
    return Sender(&q);
  }

  Dispatcher wait() {
    return Dispatcher(&q);
  }
};


}

#endif