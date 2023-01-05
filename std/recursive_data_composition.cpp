// from: https://gist.github.com/dgski/d00303b4a8be2d3c109d7a97d77106a3#file-recursive_data_composition-cpp
#include <iostream>
#include <variant>
#include <vector>

// 前方宣言
struct Node;

// シンプルデータ型
using Int = int;
using String = std::string;
using Null = std::monostate;

// 複合データ型
using Sequence = std::vector<Node>;
using Data = std::variant<Int, String, Null, Sequence>;

struct Node {
  std::string m_name;
  Data m_data;

  explicit Node(std::string &&name, Data &&data) : m_name(std::move(name)), m_data(std::move(data)) {}

  explicit Node(Data &&data) : m_data(data) {}
};

template<typename Stream>
Stream &operator<<(Stream &stream, const Null &null) {
  stream << "null";
  return stream;
}


template<typename Stream>
Stream &operator<<(Stream &stream, const Data &data) {
  std::visit([&](auto &val) {
    stream << val;
  }, data);
  return stream;
}

template<typename Stream>
Stream &operator<<(Stream &stream, const Node &node) {
  std::cout
    << "Node{ name='" << node.m_name
    << "' data=" << node.m_data << " }";
  return stream;
}

template<typename Stream>
Stream &operator<<(Stream &stream, const Sequence &data) {
  std::cout << "Sequence[ ";
  for (const auto &d: data) {
    std::cout << d << ' ';
  }
  std::cout << ']';
  return stream;
}

template<typename T, typename Func>
Sequence vecToSeq(const std::vector<T> &vec, Func func) {
  Sequence result;
  for (const auto &v: vec) {
    result.push_back(func(v));
  }
  return result;
}


void printNodes(const Node &node) {
  struct MyVisiter {
    void operator()(const Sequence &seq) {
      for (const auto &d: seq) {
        std::cout << d.m_name << std::endl;
        printNodes(d);
      }
    }

    void operator()(Int i) {
      std::cout << "Int:" << i << std::endl;
    }

    void operator()(const String &s) {
      std::cout << "String:" << s << std::endl;
    }

    void operator()(Null) {
      std::cout << "Null" << std::endl;
    }
  };
  std::visit(MyVisiter{}, node.m_data);
}

// Recursive Variant
struct RecursiveVariant;

struct RecursiveVariant {
  using Value = std::variant<int,
    std::string,
    std::unique_ptr<RecursiveVariant> >;
  Value value;
};

void foo() {
  RecursiveVariant rv = {1};
}

int main() {
  auto root =
    Node("addUserRequest",
         Sequence({
                    Node("userId", 123),
                    Node("name", "Charles"),
                    Node("age", 424),
                    Node("sessionInfo", Sequence({
                                                   Node("signOnId", "f1f133112"),
                                                   Node("bannerId", Null())
                                                 }))}));

  std::cout << root << std::endl;

  auto root2 =
    Node("test",
         Sequence({
                    Node("name", "Herbert"),
                    Node("age", 55)}));

  std::cout << root2 << std::endl;

  printNodes(root2);

  struct Custom {
    int id;
    std::string name;
  };

  std::vector<Custom> vec = {
    {12,  "Johnny"},
    {344, "Filber"},
    {999, "Jennifer"}
  };

  auto root3 =
    Node("data", Sequence({
                            Node("customThings", vecToSeq(vec, [](const Custom &c) {
                              return Node("custom", Sequence({
                                                               Node("id", c.id),
                                                               Node("name", c.name)}));
                            })),
                            Node("requestId", 232324)}));

  std::cout << root3 << std::endl;

  /*
  {
      using namespace std;
      struct PrintVisitor {
          void operator()(int x) {
              cout << "int値 : " << x << endl;
          }
          void operator()(char x) {
              cout << "char値 : " << x << endl;
          }
          void operator()(const string& x) {
              cout << "string値 : " << x << endl;
          }
      };
      variant<int, char, string> v = 3;
      // 代入されている値を出力する
      visit(PrintVisitor{}, v);
      // 文字列を代入し、代入されている値を出力する
      v = "Hello";
      visit(PrintVisitor{}, v);
      v = 'a';
      visit(PrintVisitor{}, v);
  }
  */

  return 0;
}