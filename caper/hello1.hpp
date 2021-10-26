#ifndef HELLO1_HPP_
#define HELLO1_HPP_

// This file was automatically generated by Caper.
// (http://jonigata.github.io/caper/caper.html)

#include <cstdlib>
#include <cassert>

namespace hello_world {

enum Token {
    token_eof,
    token_Hello,
    token_World,
};

inline const char* token_label(Token t) {
    static const char* labels[] = {
        "token_eof",
        "token_Hello",
        "token_World",
    };
    return labels[t];
}

template <class T, unsigned int StackSize>
class Stack {
public:
    Stack() { top_ = 0; gap_ = 0; tmp_ = 0; }
    ~Stack() {}

    void rollback_tmp() {
        for (size_t i = 0 ; i <tmp_ ; i++) {
            at(StackSize - 1 - i).~T(); // explicit destructor
        }
        tmp_ = 0;
        gap_ = top_;
    }

    void commit_tmp() {
        for (size_t i = 0 ; i <tmp_ ; i++) {
            if (gap_ + i <top_) {
                at(gap_ + i) = at(StackSize - 1 - i);
            } else {
                new (&at(gap_ + i)) T(at(StackSize - 1 - i));
            }
            at(StackSize - 1 - i).~T(); // explicit destructor
        }
        if (gap_ + tmp_ <top_) {
            for (int i = 0 ; i <int(top_ - gap_ - tmp_); i++) {
                at(top_ - 1 - i).~T(); // explicit destructor
            }
        }

        top_ = gap_ = gap_ + tmp_;
        tmp_ = 0;
    }

    bool push(const T& f) {
        if (StackSize <= top_ + tmp_) { return false; }
        new (&at(StackSize - 1 - tmp_++)) T(f);
        return true;
    }

    void pop(size_t n) {
        size_t m = n; if (m > tmp_) { m = tmp_; }

        for (size_t i = 0 ; i <m ; i++) {
            at(StackSize - tmp_ + i).~T(); // explicit destructor
        }

        tmp_ -= m;
        gap_ -= n - m;
    }

    T& top() {
        assert(0 < depth());
        if (0 <tmp_) {
            return at(StackSize - 1 -(tmp_-1));
        } else {
            return at(gap_ - 1);
        }
    }

    const T& get_arg(size_t base, size_t index) {
        if (base - index <= tmp_) {
            return at(StackSize-1 - (tmp_ -(base - index)));
        } else {
            return at(gap_ -(base - tmp_) + index);
        }
    }

    void clear() {
        rollback_tmp();
        for (size_t i = 0 ; i <top_ ; i++) {
            at(i).~T(); // explicit destructor
        }
        top_ = gap_ = tmp_ = 0;
    }

    bool empty() const {
        if (0 < tmp_) {
            return false;
        } else {
            return gap_ == 0;
        }
    }

    size_t depth() const {
        return gap_ + tmp_;
    }

    T& nth(size_t index) {
        if (gap_ <= index) {
            return at(StackSize-1 - (index - gap_));
        } else {
            return at(index);
        }
    }

    void swap_top_and_second() {
        int d = depth();
        assert(2 <= d);
        T x = nth(d - 1);
        nth(d - 1) = nth(d - 2);
        nth(d - 2) = x;
    }

private:
    T& at(size_t n) {
        return *(T*)(stack_ + (n * sizeof(T)));
    }

private:
    char stack_[ StackSize * sizeof(T) ];
    size_t top_;
    size_t gap_;
    size_t tmp_;

};

template <class Value, class SemanticAction,
          unsigned int StackSize = 1024>
class Parser {
public:
    typedef Token token_type;
    typedef Value value_type;

    enum Nonterminal {
        Nonterminal_HelloWorld,
    };

public:
    Parser(SemanticAction& sa) : sa_(sa) { reset(); }

    void reset() {
        error_ = false;
        accepted_ = false;
        clear_stack();
        rollback_tmp_stack();
        if (push_stack(0, value_type())) {
            commit_tmp_stack();
        } else {
            sa_.stack_overflow();
            error_ = true;
        }
    }

    bool post(token_type token, const value_type& value) {
        rollback_tmp_stack();
        error_ = false;
        while ((this->*(stack_top()->entry->state))(token, value))
            ; // may throw
        if (!error_) {
            commit_tmp_stack();
        } else {
            recover(token, value);
        }
        return accepted_ || error_;
    }

    bool accept(value_type& v) {
        assert(accepted_);
        if (error_) { return false; }
        v = accepted_value_;
        return true;
    }

    bool error() { return error_; }

private:
    typedef Parser<Value, SemanticAction, StackSize> self_type;

    typedef bool (self_type::*state_type)(token_type, const value_type&);
    typedef int (self_type::*gotof_type)(Nonterminal);

    bool            accepted_;
    bool            error_;
    value_type      accepted_value_;
    SemanticAction& sa_;

    struct table_entry {
        state_type  state;
        gotof_type  gotof;
        bool        handle_error;
    };

    struct stack_frame {
        const table_entry*  entry;
        value_type          value;
        int                 sequence_length;

        stack_frame(const table_entry* e, const value_type& v, int sl)
            : entry(e), value(v), sequence_length(sl) {}
    };

    Stack<stack_frame, StackSize> stack_;

    bool push_stack(int state_index, const value_type& v, int sl = 0) {
        bool f = stack_.push(stack_frame(entry(state_index), v, sl));
        assert(!error_);
        if (!f) { 
            error_ = true;
            sa_.stack_overflow();
        }
        return f;
    }

    void pop_stack(size_t n) {
        stack_.pop(n);
    }

    stack_frame* stack_top() {
        return &stack_.top();
    }

    const value_type& get_arg(size_t base, size_t index) {
        return stack_.get_arg(base, index).value;
    }

    void clear_stack() {
        stack_.clear();
    }

    void rollback_tmp_stack() {
        stack_.rollback_tmp();
    }

    void commit_tmp_stack() {
        stack_.commit_tmp();
    }

    void recover(Token, const value_type&) {
    }

    bool call_nothing(Nonterminal nonterminal, int base) {
        pop_stack(base);
        int dest_index = (this->*(stack_top()->entry->gotof))(nonterminal);
        return push_stack(dest_index, value_type());
    }

    bool call_0_Greet(Nonterminal nonterminal, int base) {
        int r = sa_.Greet();
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        int dest_index = (this->*(stack_top()->entry->gotof))(nonterminal);
        return push_stack(dest_index, v);
    }

    bool state_0(token_type token, const value_type& value) {
        switch(token) {
        case token_Hello:
            // shift
            push_stack(/*state*/ 2, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    int gotof_0(Nonterminal nonterminal) {
        switch(nonterminal) {
        case Nonterminal_HelloWorld: return 1;
        default: assert(0); return false;
        }
    }

    bool state_1(token_type token, const value_type& value) {
        switch(token) {
        case token_eof:
            // accept
            accepted_ = true;
            accepted_value_ = get_arg(1, 0);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    int gotof_1(Nonterminal nonterminal) {
        assert(0);
        return true;
    }

    bool state_2(token_type token, const value_type& value) {
        switch(token) {
        case token_World:
            // shift
            push_stack(/*state*/ 3, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    int gotof_2(Nonterminal nonterminal) {
        assert(0);
        return true;
    }

    bool state_3(token_type token, const value_type& value) {
        switch(token) {
        case token_eof:
            // reduce
            return call_0_Greet(Nonterminal_HelloWorld, /*pop*/ 2);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    int gotof_3(Nonterminal nonterminal) {
        assert(0);
        return true;
    }

    const table_entry* entry(int n) const {
        static const table_entry entries[] = {
            { &Parser::state_0, &Parser::gotof_0, false },
            { &Parser::state_1, &Parser::gotof_1, false },
            { &Parser::state_2, &Parser::gotof_2, false },
            { &Parser::state_3, &Parser::gotof_3, false },
        };
        return &entries[n];
    }

};

} // namespace hello_world

#endif // #ifndef HELLO1_HPP_
