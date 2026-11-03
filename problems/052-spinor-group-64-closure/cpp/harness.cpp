#include <cassert>
#include <chrono>
#include <iostream>
#include <set>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 052 Order-64 Spinor Group Closure Test Suite ===\n";

    SpinorGroup64 grp = build_spinor_group_64();

    // Test 1: Group closure (64 distinct states) and identity inverses
    std::set<Mat8> unique_states(grp.states.begin(), grp.states.end());
    assert(unique_states.size() == 64);
    for (uint8_t a = 0; a < 64; ++a) {
        uint8_t inv = grp.inverse[a];
        assert(grp.compose[a][inv] == 0);
        assert(grp.compose[inv][a] == 0);
    }
    std::cout << "  • Test 1: 64 distinct spinor states and exact two-sided inverses ... [PASS]\n";

    // Test 2: Associativity of Cayley table compose[compose[a][b]][c] == compose[a][compose[b][c]]
    for (uint8_t a = 0; a < 64; a += 5) {
        for (uint8_t b = 0; b < 64; b += 7) {
            for (uint8_t c = 0; c < 64; c += 9) {
                assert(grp.compose[grp.compose[a][b]][c] == grp.compose[a][grp.compose[b][c]]);
            }
        }
    }
    std::cout << "  • Test 2: Cayley table associativity ... [PASS]\n";

    // Test 3: Homomorphism of spinor_action: action(compose[a][b], x) == action(a, action(b, x))
    int32_t x[8] = {1, -2, 3, -4, 5, -6, 7, -8};
    int32_t tmp[8]{}, y1[8]{}, y2[8]{};
    for (uint8_t a = 0; a < 64; a += 3) {
        for (uint8_t b = 0; b < 64; b += 5) {
            spinor_action(grp, b, x, tmp);
            spinor_action(grp, a, tmp, y1);
            spinor_action(grp, grp.compose[a][b], x, y2);
            for (int d = 0; d < 8; ++d) {
                assert(y1[d] == y2[d]);
            }
        }
    }
    std::cout << "  • Test 3: Compressed axes action homomorphism ... [PASS]\n";

    constexpr int ITERS = 200000;
    volatile int32_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        spinor_action(grp, static_cast<uint8_t>(i & 63), x, y1);
        sink = sink + y1[0] + y1[7];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
