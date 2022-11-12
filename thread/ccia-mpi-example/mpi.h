#ifndef MPI_H
#define MPI_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>

namespace messaging {


struct MessageBase {
  virtual ~MessageBase() = default;
};

template<typename Msg>
struct WrappedMessage : MessageBase {
  Msg contents;
  explicit WrappedMessage(Msg const &contents_) : contents(contents_) {}
};

class Queue {
  std::mutex m;
  std::condition_variable c;
  std::queue<std::shared_ptr<MessageBase> > q;

public:
  template<typename T>
  void push(T const &msg) {
	std::lock_guard<std::mutex> lk(m);
	q.push(std::make_shared<WrappedMessage<T> >(msg));
	c.notify_all();
  }

  std::shared_ptr<MessageBase> wait_and_pop() {
	std::unique_lock<std::mutex> lk(m);
	c.wait(lk, [&] { return !q.empty(); });
	auto res = q.front();
	q.pop();
	return res;
  }
};

class CloseQueue {};

template<typename PreviousDispatcher, typename Msg, typename Func>
class TemplateDispatcher {
  Queue *q;
  PreviousDispatcher *prev;
  Func f;
  bool chained;

  template<typename Dispatcher, typename OtherMsg, typename OtherFunc>
  friend class TemplateDispatcher;

  void wait_and_dispatch() {
	for (;;) {
	  auto msg = q->wait_and_pop();
	  if (dispatch(msg))
		break;
	}
  }

  bool dispatch(std::shared_ptr<MessageBase> const &msg) {
	if (auto *wrapper = dynamic_cast<WrappedMessage<Msg> *>(msg.get())) {
	  f(wrapper->contents);
	  return true;
	} else {
	  return prev->dispatch(msg);
	}
  }

public:
  TemplateDispatcher(TemplateDispatcher const &) = delete;
  TemplateDispatcher &operator=(TemplateDispatcher const &) = delete;
  TemplateDispatcher(TemplateDispatcher &&other)  noexcept : q(other.q), prev(other.prev), f(std::move(other.f)), chained(other.chained) {
	other.chained = true;
  }

  TemplateDispatcher(Queue *q_, PreviousDispatcher *prev_, Func &&f_) : q(q_), prev(prev_), f(std::forward<Func>(f_)), chained(false) {
	prev_->chained = true;
  }

  template<typename OtherMsg, typename OtherFunc>
  TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>
  handle(OtherFunc &&of) {
	return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>( q, this, std::forward<OtherFunc>(of));
  }

  ~TemplateDispatcher() noexcept(false) {
	if (!chained)
	  wait_and_dispatch();
  }
};

class Dispatcher {
  Queue *q;
  bool chained;

  template<typename Dispatcher, typename Msg, typename Func>
  friend class TemplateDispatcher;

  [[noreturn]] void wait_and_dispatch() {
	for (;;) {
	  auto msg = q->wait_and_pop();
	  dispatch(msg);
	}
  }

  bool dispatch(std::shared_ptr<MessageBase> const &msg) {
	if (dynamic_cast<WrappedMessage<CloseQueue> *>(msg.get())) {
	  throw CloseQueue();
	}
	return false;
  }

 public:
  Dispatcher(Dispatcher const &) = delete;
  Dispatcher &operator=(Dispatcher const &) = delete;
  Dispatcher(Dispatcher &&other) noexcept : q(other.q), chained(other.chained) {
	other.chained = true;
  }

  explicit Dispatcher(Queue *q_) : q(q_), chained(false) {}

  template<typename Message, typename Func>
  TemplateDispatcher<Dispatcher, Message, Func>
  handle(Func &&f) {
	return TemplateDispatcher<Dispatcher, Message, Func>(
		q, this, std::forward<Func>(f));
  }

  ~Dispatcher() noexcept(false) {
	if (!chained) {
	  wait_and_dispatch();
	}
  }
};

class Sender {
  Queue *q;

 public:
  Sender() : q(nullptr) {}
  explicit Sender(Queue *q_) : q(q_) {}

  template<typename Message>
  void send(Message const &msg) {
	if (q)
	  q->push(msg);
  }
};


class Receiver {
  Queue q;

 public:
  operator Sender() {
	return Sender(&q);
  }

  Dispatcher wait() {
	return Dispatcher(&q);
  }
};


}

#endif