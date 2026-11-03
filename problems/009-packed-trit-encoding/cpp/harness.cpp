#include <array>
#include <bit>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 009 Packed 2-Bit Trit Encoding Test Suite ===\n";

    // Test 1: Validity invariant check (pos & neg) == 0
    PackedTrit64 z{0ULL, 0ULL};
    assert(z.is_valid());
    assert(z.nnz() == 0);

    PackedTrit64 bad{0x10ULL, 0x10ULL};
    assert(!bad.is_valid());
    std::cout << "  • Test 1: Disjoint plane validity check (P & N == 0) ... [PASS]\n";

    // Test 2: Round-trip pack/unpack and L1 support identity
    std::array<int8_t, 64> arr{};
    size_t expected_nnz = 0;
    for (size_t i = 0; i < 64; ++i) {
        int m = static_cast<int>(i % 3);
        arr[i] = (m == 0) ? 0 : (m == 1 ? 1 : -1);
        if (arr[i] != 0) ++expected_nnz;
    }
    PackedTrit64 pt = PackedTrit64::from_array(arr);
    assert(pt.is_valid());
    assert(pt.nnz() == expected_nnz);
    assert(pt.nnz() == static_cast<size_t>(std::popcount(pt.pos) + std::popcount(pt.neg)));
    assert(pt.to_array() == arr);
    std::cout << "  • Test 2: Round-trip pack/unpack & L1 norm conservation ... [PASS]\n";

    // Test 3: O(1) Negation involution and element-wise sign flip
    PackedTrit64 neg_pt = pt.negate();
    assert(neg_pt.is_valid());
    assert(neg_pt.nnz() == pt.nnz());
    assert(neg_pt.negate().pos == pt.pos && neg_pt.negate().neg == pt.neg);
    auto neg_arr = neg_pt.to_array();
    for (size_t i = 0; i < 64; ++i) {
        assert(neg_arr[i] == -arr[i]);
    }
    std::cout << "  • Test 3: O(1) plane-swap negation & involution ... [PASS]\n";

    // Timing benchmark
    constexpr size_t ITERS = 1000000;
    size_t acc = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        PackedTrit64 tmp{pt.pos ^ (i & 0x5555ULL), pt.neg & ~(i & 0x5555ULL)};
        acc += tmp.negate().nnz();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    if (acc == 0xdeadbeefULL) std::cout << acc;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);

    std::cout << "All 009 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
