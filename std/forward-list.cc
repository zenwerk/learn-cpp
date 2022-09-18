#include <forward_list>
#include <iostream>
#include <memory>

class Foo {
public:
  int val_;

  explicit Foo(int val) : val_(val) {}

  // sort() するなら必要
  bool operator<(const Foo &rhs) const { return val_ < rhs.val_; }
};

int main() {
  auto foo1 = std::make_shared<Foo>(1);
  auto foo2 = std::make_shared<Foo>(2);
  auto foo3 = std::make_shared<Foo>(3);
  auto foo4 = std::make_shared<Foo>(4);

  // forward_list を用意
  std::forward_list<std::shared_ptr<Foo>> foo_list;
  foo_list.push_front(std::move(foo1));
  foo_list.push_front(std::move(foo2));
  foo_list.insert_after(foo_list.begin(), std::move(foo3));

  // 最後に挿入
  auto before_end = foo_list.before_begin();
  for (auto &_ : foo_list)
    ++before_end;
  foo_list.insert_after(before_end, std::move(foo4));

  // for loop で出力
  for (auto itr = foo_list.cbegin(); itr != foo_list.cend(); ++itr) {
    std::cout << *itr << " : val_ = " << itr->get()->val_ << std::endl;
  }

  // for_each で出力
  std::for_each(foo_list.cbegin(), foo_list.cend(), [](auto foo) {
    std::cout << "val_ = " << foo->val_ << std::endl;
  });

  // ソート
  foo_list.sort();
  std::cout << "Sorted!!" << std::endl;
  std::for_each(foo_list.cbegin(), foo_list.cend(), [](auto foo) {
    std::cout << "val_ = " << foo->val_ << std::endl;
  });

  // 要素の削除
  std::cout << "erase elements" << std::endl;
  for (auto itr = foo_list.begin(); itr != foo_list.end(); ++itr) {
    if (itr->get()->val_ == 1) {
      foo_list.erase_after(itr);
    }
    if (itr->get()->val_ == 3) {
      // foo_list.remove(itr->get()); TODO: スマートポインタで remove するには？
    }
  }

  std::for_each(foo_list.cbegin(), foo_list.cend(), [](auto foo) {
    std::cout << "val_ = " << foo->val_ << std::endl;
  });

  return 0;
}