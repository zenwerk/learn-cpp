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
    explicit Node(std::string&& name, Data&& data) : m_name(std::move(name)), m_data(std::move(data)) {}
    explicit Node(Data&& data) : m_data(data) {}
};

template<typename Stream>
Stream& operator<<(Stream& stream, const Null& null) {
    stream << "null";
    return stream;
}


template<typename Stream>
Stream& operator<<(Stream& stream, const Data& data) {
    std::visit([&](auto& val) {
        stream << val;
    }, data);
    return stream;
}

template<typename Stream>
Stream& operator<<(Stream& stream, const Node& node) {
    std::cout
        << "Node{ name='" << node.m_name
        << "' data=" << node.m_data << " }";
    return stream;
}

template<typename Stream>
Stream& operator<<(Stream& stream, const Sequence& data) {
    std::cout << "Sequence[ ";
    for(const auto& d : data) {
        std::cout << d << ' ';
    }
    std::cout << ']';
    return stream;
}

template<typename T, typename Func>
Sequence vecToSeq(const std::vector<T>& vec, Func func) {
    Sequence result;
    for(const auto& v : vec) {
        result.push_back(func(v));
    }
    return result;
}

struct Visiter {
    void operator()(Int& n) {
    }
};


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

    Visiter v;
    v(Data{1});

    auto root2 =
        Node("test",
            Sequence({
                Node("name", "Herbert"),
                Node("age", 55)}));

    std::cout << root2 << std::endl;

    struct Custom {
        int id;
        std::string name;
    };

    std::vector<Custom> vec = {
        {12, "Johnny"}, { 344, "Filber"}, {999, "Jennifer"}
    };

    auto root3 =
        Node("data", Sequence({
            Node("customThings", vecToSeq(vec, [](const Custom& c) {
                return Node("custom", Sequence({
                    Node("id", c.id),
                    Node("name", c.name)})); })),
            Node("requestId", 232324)}));

    std::cout << root3 << std::endl;

    return 0;
}