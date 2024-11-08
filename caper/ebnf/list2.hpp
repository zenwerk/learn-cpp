#ifndef LIST2_HPP_
#define LIST2_HPP_

// This file was automatically generated by Caper.
// (http://jonigata.github.io/caper/caper.html)

#include <cstdlib>
#include <cassert>

namespace list {

enum Token {
    token_eof,
    token_error,
    token_Comma,
    token_LParen,
    token_Number,
    token_RParen,
    token_Star,
};

inline const char* token_label(Token t) {
    static const char* labels[] = {
        "token_eof",
        "token_error",
        "token_Comma",
        "token_LParen",
        "token_Number",
        "token_RParen",
        "token_Star",
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
        Nonterminal_Document,
        Nonterminal_Number_seq0,
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
        int nn = int(n);
        while(nn--) {
            stack_.pop(1 + stack_.top().sequence_length);
        }
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

    void recover(Token token, const value_type& value) {
        rollback_tmp_stack();
        error_ = false;
        while(!stack_top()->entry->handle_error) {
            pop_stack(1);
            if (stack_.empty()) {
                error_ = true;
                return;
            }
        }
        // post error_token;
        while ((this->*(stack_top()->entry->state))(token_error, value_type()));
        commit_tmp_stack();
        // repost original token
        // if it still causes error, discard it;
        while ((this->*(stack_top()->entry->state))(token, value));
        if (!error_) {
            commit_tmp_stack();
        }
        if (token != token_eof) {
            error_ = false;
        }
    }

    // EBNF support class
    struct Range {
        int beg;
        int end;
        Range() : beg(-1), end(-1) {}
        Range(int b, int e) : beg(b), end(e) {}
    };

    template <class T>
    class Optional {
    public:
        typedef Stack<stack_frame, StackSize> stack_type;

    public:
        Optional(SemanticAction& sa, stack_type& s, const Range& r)
            : sa_(&sa), s_(&s), p_(r.beg == r.end ? -1 : r.beg){}

        operator bool() const {
            return 0 <= p_;
        }
        bool operator!() const {
            return !bool(*this);
        }
        T operator*() const {
            value_type v;
            sa_->downcast(v, s_->nth(p_).value);
            return v;
        }

    private:
        SemanticAction* sa_;
        stack_type*     s_;
        int             p_;

    };

    template <class T>
    class Sequence {
    public:
        typedef Stack<stack_frame, StackSize> stack_type;

        class const_iterator {
        public:
            typedef T value_type;

        public:
            const_iterator(SemanticAction& sa, stack_type& s, int p)
                : sa_(&sa), s_(&s), p_(p){}
            const_iterator(const const_iterator& x) : s_(x.s_), p_(x.p_){}
            const_iterator& operator=(const const_iterator& x) {
                sa_ = x.sa_;
                s_ = x.s_;
                p_ = x.p_;
                return *this;
            }
            value_type operator*() const {
                value_type v;
                sa_->downcast(v, s_->nth(p_).value);
                return v;
            }
            const_iterator& operator++() {
                ++p_;
                return *this;
            }
            bool operator==(const const_iterator& x) const {
                return p_ == x.p_;
            }
            bool operator!=(const const_iterator& x) const {
                return !((*this)==x);
            }
        private:
            SemanticAction* sa_;
            stack_type*     s_;
            int             p_;

        };

    public:
        Sequence(SemanticAction& sa, stack_type& stack, const Range& r)
            : sa_(sa), stack_(stack), range_(r) {
        }

        const_iterator begin() const {
            return const_iterator(sa_, stack_, range_.beg);
        }
        const_iterator end() const {
            return const_iterator(sa_, stack_, range_.end);
        }

    private:
        SemanticAction& sa_;
        stack_type&     stack_;
        Range           range_;

    };

    // EBNF support member functions
    bool seq_head(Nonterminal nonterminal, int base) {
        // case '*': base == 0
        // case '+': base == 1
        int dest = (this->*(stack_nth_top(base)->entry->gotof))(nonterminal);
        return push_stack(dest, value_type(), base);
    }

    bool seq_trail(Nonterminal, int base) {
        // '*', '+' trailer
        assert(base == 2);
        stack_.swap_top_and_second();
        stack_top()->sequence_length++;
        return true;
    }

    bool seq_trail2(Nonterminal, int base) {
        // '/' trailer
        assert(base == 3);
        stack_.swap_top_and_second();
        pop_stack(1); // erase delimiter
        stack_.swap_top_and_second();
        stack_top()->sequence_length++;
        return true;
    }

    bool opt_nothing(Nonterminal nonterminal, int base) {
        // same as head of '*'
        assert(base == 0);
        return seq_head(nonterminal, base);
    }

    bool opt_just(Nonterminal nonterminal, int base) {
        // same as head of '+'
        assert(base == 1);
        return seq_head(nonterminal, base);
    }

    Range seq_get_range(size_t base, size_t index) {
        // returns beg = end if length = 0 (includes scalar value)
        // distinguishing 0-length-vector against scalar value is
        // caller's responsibility
        int n = int(base - index);
        assert(0 < n);
        int prev_actual_index;
        int actual_index  = stack_.depth();
        while(n--) {
            actual_index--;
            prev_actual_index = actual_index;
            actual_index -= stack_.nth(actual_index).sequence_length;
        }
        return Range(actual_index, prev_actual_index);
    }

    const value_type& seq_get_arg(size_t base, size_t index) {
        Range r = seq_get_range(base, index);
        // multiple value appearing here is not supported now
        assert(r.end - r.beg == 0); 
        return stack_.nth(r.beg).value;
    }

    stack_frame* stack_nth_top(int n) {
        Range r = seq_get_range(n + 1, 0);
        // multiple value appearing here is not supported now
        assert(r.end - r.beg == 0);
        return &stack_.nth(r.beg);
    }
    bool call_nothing(Nonterminal nonterminal, int base) {
        pop_stack(base);
        int dest_index = (this->*(stack_top()->entry->gotof))(nonterminal);
        return push_stack(dest_index, value_type());
    }

    bool call_0_Document(Nonterminal nonterminal, int base, int arg_index0) {
        Sequence<int> arg0(sa_, stack_, seq_get_range(base, arg_index0)); 
        int r = sa_.Document(arg0);
        value_type v; sa_.upcast(v, r);
        pop_stack(base);
        int dest_index = (this->*(stack_top()->entry->gotof))(nonterminal);
        return push_stack(dest_index, v);
    }

    bool state_0(token_type token, const value_type& value) {
        switch(token) {
        case token_LParen:
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
        case Nonterminal_Document: return 1;
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
        case token_Number:
            // shift
            push_stack(/*state*/ 5, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    int gotof_2(Nonterminal nonterminal) {
        switch(nonterminal) {
        case Nonterminal_Number_seq0: return 3;
        default: assert(0); return false;
        }
    }

    bool state_3(token_type token, const value_type& value) {
        switch(token) {
        case token_Comma:
            // shift
            push_stack(/*state*/ 6, value);
            return false;
        case token_RParen:
            // shift
            push_stack(/*state*/ 4, value);
            return false;
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

    bool state_4(token_type token, const value_type& value) {
        switch(token) {
        case token_eof:
            // reduce
            return call_0_Document(Nonterminal_Document, /*pop*/ 3, 1);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    int gotof_4(Nonterminal nonterminal) {
        assert(0);
        return true;
    }

    bool state_5(token_type token, const value_type& value) {
        switch(token) {
        case token_Comma:
            // reduce
            return seq_head(Nonterminal_Number_seq0, /*pop*/ 1);
        case token_RParen:
            // reduce
            return seq_head(Nonterminal_Number_seq0, /*pop*/ 1);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    int gotof_5(Nonterminal nonterminal) {
        assert(0);
        return true;
    }

    bool state_6(token_type token, const value_type& value) {
        switch(token) {
        case token_Number:
            // shift
            push_stack(/*state*/ 7, value);
            return false;
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    int gotof_6(Nonterminal nonterminal) {
        assert(0);
        return true;
    }

    bool state_7(token_type token, const value_type& value) {
        switch(token) {
        case token_Comma:
            // reduce
            return seq_trail2(Nonterminal_Number_seq0, /*pop*/ 3);
        case token_RParen:
            // reduce
            return seq_trail2(Nonterminal_Number_seq0, /*pop*/ 3);
        default:
            sa_.syntax_error();
            error_ = true;
            return false;
        }
    }

    int gotof_7(Nonterminal nonterminal) {
        assert(0);
        return true;
    }

    const table_entry* entry(int n) const {
        static const table_entry entries[] = {
            { &Parser::state_0, &Parser::gotof_0, false },
            { &Parser::state_1, &Parser::gotof_1, false },
            { &Parser::state_2, &Parser::gotof_2, false },
            { &Parser::state_3, &Parser::gotof_3, false },
            { &Parser::state_4, &Parser::gotof_4, false },
            { &Parser::state_5, &Parser::gotof_5, false },
            { &Parser::state_6, &Parser::gotof_6, false },
            { &Parser::state_7, &Parser::gotof_7, false },
        };
        return &entries[n];
    }

};

} // namespace list

#endif // #ifndef LIST2_HPP_

