#include <bit>
#include <cassert>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running Rule 184 Demarcation Test Suite ===\n";

    // Periodic test: left_bit comes from bit 63, right_bit comes from bit 0
    uint64_t initial = 0xAAAAAAAAAAAAAAAAULL; // alternating 101010... (32 particles)
    uint64_t left_bit = (initial >> 63) & 1;
    uint64_t right_bit = initial & 1;

    uint64_t next_state = step_rule184(initial, left_bit, right_bit);

    // Particle conservation check: total set bits must remain identical
    assert(std::popcount(initial) == std::popcount(next_state));
    std::cout << "  • Test 1: Periodic 64-bit particle count conservation ... [PASS]\n";

    std::cout << "All Rule 184 tests passed successfully!\n";
    return 0;
}
