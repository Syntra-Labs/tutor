#pragma once
#pragma GCC diagnostic ignored "-Wparentheses"
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <cstdlib>

namespace _tutor_check {
template <typename T>
std::string fmt(const T& v) {
    using D = std::remove_cvref_t<T>;
    if constexpr (std::is_same_v<D, bool>) {
        return v ? "true" : "false";
    } else if constexpr (std::is_convertible_v<D, std::string_view>) {
        std::string s{std::string_view(v)};
        std::string out = "\"";
        for (char c : s) {
            if (c == '"') out += "\\\"";
            else if (c == '\\') out += "\\\\";
            else if (c == '\n') out += "\\n";
            else out += c;
        }
        out += "\"";
        return out;
    } else if constexpr (std::is_same_v<D, char> || std::is_same_v<D, signed char> || std::is_same_v<D, unsigned char>) {
        return std::to_string(static_cast<int>(v));
    } else if constexpr (requires(std::ostream& os, const D& x) { os << x; }) {
        std::ostringstream oss;
        oss << v;
        return oss.str();
    } else {
        return "<object>";
    }
}

inline std::string json_escape(std::string_view s) {
    std::string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

struct EvalResult {
    bool ok;
    std::string actual;
    std::string expected;
    template <typename R>
    EvalResult operator&&(const R& rhs) const {
        bool b = ok && static_cast<bool>(rhs);
        return EvalResult{b, b ? "true" : "false", "true"};
    }
    template <typename R>
    EvalResult operator||(const R& rhs) const {
        bool b = ok || static_cast<bool>(rhs);
        return EvalResult{b, b ? "true" : "false", "true"};
    }
};

template <typename L>
struct LhsWrap {
    const L& lhs;
    operator EvalResult() const {
        bool b = static_cast<bool>(lhs);
        return EvalResult{b, b ? "true" : "false", "true"};
    }
    template <typename R>
    EvalResult operator==(const R& rhs) const {
        bool b = (lhs == rhs);
        return EvalResult{b, fmt(lhs), fmt(rhs)};
    }
    template <typename R>
    EvalResult operator!=(const R& rhs) const {
        bool b = (lhs != rhs);
        return EvalResult{b, fmt(lhs), "!= " + fmt(rhs)};
    }
    template <typename R>
    EvalResult operator<(const R& rhs) const {
        bool b = (lhs < rhs);
        return EvalResult{b, fmt(lhs), "< " + fmt(rhs)};
    }
    template <typename R>
    EvalResult operator<=(const R& rhs) const {
        bool b = (lhs <= rhs);
        return EvalResult{b, fmt(lhs), "<= " + fmt(rhs)};
    }
    template <typename R>
    EvalResult operator>(const R& rhs) const {
        bool b = (lhs > rhs);
        return EvalResult{b, fmt(lhs), "> " + fmt(rhs)};
    }
    template <typename R>
    EvalResult operator>=(const R& rhs) const {
        bool b = (lhs >= rhs);
        return EvalResult{b, fmt(lhs), ">= " + fmt(rhs)};
    }
    template <typename R>
    EvalResult operator&&(const R& rhs) const {
        bool b = static_cast<bool>(lhs) && static_cast<bool>(rhs);
        return EvalResult{b, b ? "true" : "false", "true"};
    }
    template <typename R>
    EvalResult operator||(const R& rhs) const {
        bool b = static_cast<bool>(lhs) || static_cast<bool>(rhs);
        return EvalResult{b, b ? "true" : "false", "true"};
    }
};

struct Decomposer {
    template <typename T>
    LhsWrap<T> operator<=(const T& lhs) const {
        return LhsWrap<T>{lhs};
    }
};

inline void run_assert(const char* expr_str, const EvalResult& res) {
    static int emitted = 0;
    static const char* last_expr = nullptr;
    std::string expr(expr_str);
    std::string input_label = expr;
    auto eq_pos = expr.find("==");
    if (eq_pos != std::string::npos) {
        input_label = expr.substr(0, eq_pos);
        while (!input_label.empty() && input_label.back() == ' ') input_label.pop_back();
    }
    if (!res.ok) {
        std::cout << "\nTESTCASE_JSON:{\"passed\":false,\"input\":\"" << json_escape(input_label)
                  << "\",\"output\":\"" << json_escape(res.actual)
                  << "\",\"expected\":\"" << json_escape(res.expected) << "\"}\n";
        std::exit(1);
    } else if (emitted < 16 && last_expr != expr_str) {
        last_expr = expr_str;
        ++emitted;
        std::cout << "\nTESTCASE_JSON:{\"passed\":true,\"input\":\"" << json_escape(input_label)
                  << "\",\"output\":\"" << json_escape(res.actual)
                  << "\",\"expected\":\"" << json_escape(res.expected) << "\"}\n";
    }
}

inline void poison_exit(int code) {
    std::cerr << "\n[Security Error]: User solution called exit(" << code << "). Solution code must return to the test harness normally.\n";
    std::abort();
}
} // namespace _tutor_check
#undef assert
#define assert(expr) ::_tutor_check::run_assert(#expr, (::_tutor_check::Decomposer{} <= expr))
#define exit(c) ::_tutor_check::poison_exit(c)
#define quick_exit(c) ::_tutor_check::poison_exit(c)
#define _Exit(c) ::_tutor_check::poison_exit(c)
