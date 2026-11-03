#include "solution.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <string>

int main() {
    std::cout << "=== Running 000a Hello World & String Greeting Test Suite ===\n";

    std::cout << "  - Test 1: hello_world() exact match ... ";
    assert(hello_world() == "Hello, World!");
    std::cout << "[PASS]\n";

    std::cout << "  - Test 2: greet(name) formatting & empty fallback ... ";
    assert(greet("") == "Hello, World!");
    assert(greet("Syntra") == "Hello, Syntra!");
    assert(greet("Neovim") == "Hello, Neovim!");
    std::cout << "[PASS]\n";

    constexpr int ITERS = 100000;
    auto t0 = std::chrono::high_resolution_clock::now();
    size_t total_len = 0;
    for (int i = 0; i < ITERS; ++i) {
        total_len += hello_world().size() + greet("Tutor").size();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    assert(total_len == static_cast<size_t>(ITERS) * (13 + 13));

    double ns = std::chrono::duration<double, std::nano>(t1 - t0).count();
    double cycles_per_op = (ns * 3.2) / (ITERS * 2.0);
    std::cout << "SCOREBOARD_JSON:{\"cycles_per_op\":" << cycles_per_op << "}\n";
    std::cout << "All 000a tests passed!\n";
    return 0;
}
