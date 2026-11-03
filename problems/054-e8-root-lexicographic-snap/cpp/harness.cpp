#include <cassert>
#include <chrono>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 054 16-Root E8 Lexicographic Snapper Test Suite ===\n";

    // Test 1: Exact idempotence on all 16 roots at scale=1 and scale=3
    for (uint8_t r = 0; r < 16; ++r) {
        int32_t x1[8], x3[8];
        for (int d = 0; d < 8; ++d) {
            x1[d] = CANONICAL_E8_ORBIT_16[r][d];
            x3[d] = 3 * CANONICAL_E8_ORBIT_16[r][d];
        }
        uint32_t dist = 99, ham = 99;
        root_cost(CANONICAL_E8_ORBIT_16, x1, r, dist, ham);
        assert(dist == 0 && ham == 0);
        assert(snap_e8_root(CANONICAL_E8_ORBIT_16, x1, 1) == r);
        assert(snap_e8_root(CANONICAL_E8_ORBIT_16, x3, 3) == r);
    }
    std::cout << "  • Test 1: Exact idempotence across all 16 roots and scales ... [PASS]\n";

    // Test 2: Secondary Hamming tie-breaking when L1 distances are equal
    // Compare root 0: (2, 2, 0, 0, 0, 0, 0, 0) vs root 2: (-2, 2, 0, 0, 0, 0, 0, 0)
    // For x = (0, 2, 0, 0, 0, 0, 0, 0), both root 0 and root 2 have L1 = 2 and Hamming = 1,
    // so tie-breaker picks smaller index r = 0.
    // For x = (1, 1, 0, 0, 0, 0, 0, 0), root 0 has L1 = 2, Hamming = 2;
    // what about x = (0, 2, 1, 0, 0, 0, 0, 0)? Root 0 has L1 = 3, Hamming = 2.
    int32_t x_tie[8] = {0, 2, 0, 0, 0, 0, 0, 0};
    assert(snap_e8_root(CANONICAL_E8_ORBIT_16, x_tie, 1) == 0);

    // Construct a vector where two roots have equal L1=4, but one matches a coordinate exactly (smaller Hamming):
    // x = (2, 0, 0, 0, 0, 0, 0, 0):
    // - Against root 0 (2, 2, 0..): L1 = |2-2| + |0-2| = 2, Hamming = 1
    // - Against root 15 (2, -2, 0..): L1 = 2, Hamming = 1 -> picks root 0
    // Now take x = (0, 0, 0, 0, 0, 0, 2, 0):
    // - Root 9 is (0..0, 2, -2): L1 = 2, Hamming = 1 (matches d=6)
    // - Root 10 is (0..0, 2, 2): L1 = 2, Hamming = 1 -> picks root 9
    // Now compare against x = (1, 1, 0, 0, 0, 0, 2, 0):
    // - Root 0 (2,2,0..0): L1 = 1 + 1 + 2 = 4, Hamming = 3 (differs at d=0,1,6)
    // - Root 9 (0..0,2,-2): L1 = 1 + 1 + 0 + 2 = 4, Hamming = 3
    // Now x = (1, 1, 0, 0, 0, 0, 2, -1):
    // - Root 0 (2,2,0..0): L1 = 1+1+2+1 = 5, Hamming = 4
    // - Root 9 (0..0,2,-2): L1 = 1+1+0+1 = 3, Hamming = 3
    // What if x = (1, 1, 1, 0, 0, 0, 2, 0)?
    // - Root 0 (2,2,0,0,0,0,0,0): L1 = 1+1+1+2 = 5, Hamming = 4 (d=0,1,2,6)
    // - Root 9 (0,0,0,0,0,0,2,-2): L1 = 1+1+1+0+2 = 5, Hamming = 4 (d=0,1,2,7)
    // What if x = (2, 1, 0, 0, 0, 0, 1, 1)?
    // - Root 0 (2,2,0..0): L1 = 0 + 1 + 1 + 1 = 3, Hamming = 3 (d=1,6,7)
    // - Root 10 (0..0,2,2): L1 = 2 + 1 + 1 + 1 = 5
    // What if x = (1, 1, 0, 0, 0, 0, 2, 0)?Wait:
    // Let's make Root 10 have same L1 as Root 0, but smaller Hamming than Root 0!
    // Take x = (1, 1, 0, 0, 0, 0, 2, 0):
    //Wait: Root 0 is (2, 2, 0, 0, 0, 0, 0, 0) -> L1 = |1-2|+|1-2|+|2-0| = 4, Hamming = 3 (d=0,1,6).
    //Wait: can another root have L1 = 4 and Hamming = 2?
    // If x = (0, 0, 0, 0, 0, 0, 2, 0):
    // - Root 0 (2,2,0..0): L1 = 2+2+2 = 6
    // Wait: if x = (1, 2, 0, 0, 0, 0, 2, 1):
    // - Root 0 (2,2,0,0,0,0,0,0): L1 = 1 + 0 + 2 + 1 = 4, Hamming = 3 (d=0, 6, 7)
    // - Root 10 (0,0,0,0,0,0,2,2): L1 = 1 + 2 + 0 + 1 = 4, Hamming = 3 (d=0, 1, 7)
    // What if x = (1, 1, 1, 1, 0, 0, 2, 2)?
    // - Root 0 (2,2,0,0,0,0,0,0): L1 = 1+1+1+1+2+2 = 8, Hamming = 6
    // - Root 10 (0,0,0,0,0,0,2,2): L1 = 1+1+1+1+0+0 = 4, Hamming = 4
    // How to get equal L1 between Root 0 (2,2,0...) and Root 10 (0,...,2,2), with Root 10 having SMALLER Hamming?
    // Root 0 changes L1 by (|x0-2|+|x1-2|) - (|x0|+|x1|).
    // Root 10 changes L1 by (|x6-2|+|x7-2|) - (|x6|+|x7|).
    // Set (x0, x1) = (1, 1): for Root 0, |1-2|+|1-2| = 2 vs |1|+|1| = 2 (delta L1 = 0! And neither coordinate equals 2, so Hamming on (0,1) is 2 for BOTH Root 0 and Root 10!).
    // Set (x6, x7) = (2, 0): for Root 10, |2-2|+|0-2| = 2 vs |2|+|0| = 2 (delta L1 = 0! And x6 == 2 matches Root 10, so Hamming on (6,7) is 1 for Root 10 vs 1 for Root 0? No: for Root 0, (6,7) is (0,0), so x=(2,0) differs on d=6 (Hamming 1). Wait: on (0,1), x=(1,1) differs on BOTH d=0,1 from (0,0) (Hamming 2) AND from (2,2) (Hamming 2)!
    // Wait: what if (x0, x1) = (1, 1) and (x6, x7) = (2, 2)? Then L1 is smaller for Root 10.
    // What if (x0, x1) = (3, 1)?
    // For Root 0 (2,2): |3-2|+|1-2| = 2, Hamming on (0,1) = 2 (since 3!=2 and 1!=2).
    // For Root 10 (0,0): |3-0|+|1-0| = 4, Hamming on (0,1) = 2.
    // Then on (6,7), set (x6, x7) = (2, 1):
    // For Root 0 (0,0): |2-0|+|1-0| = 3. Total L1(Root 0) = 2 + 3 = 5.
    //Wait: if (x0, x1) = (1, 1) (delta L1 = 0) and we want Root 0 to have HIGHER Hamming on (0,1) than Root 10:
    // If x0 = 0, then x0 matches Root 10's 0!
    // Let (x0, x1) = (0, 3):
    // - Root 0 (2,2): |0-2| + |3-2| = 3; Hamming on (0,1) = 2 (0!=2, 3!=2).
    // - Root 10 (0,0): |0-0| + |3-0| = 3; Hamming on (0,1) = 1 (0==0, 3!=0)!
    // And let (x6, x7) = (2, 1):
    // - Root 0 (0,0): |2-0| + |1-0| = 3; Hamming on (6,7) = 2 (2!=0, 1!=0).
    // - Root 10 (2,2): |2-2| + |1-2| = 1; wait we want equal L1 on (6,7) too, so let (x6, x7) = (2, 0):
    // - Root 0 (0,0): |2-0| + |0-0| = 2; Hamming on (6,7) = 1.
    // - Root 10 (2,2): |2-2| + |0-2| = 2; Hamming on (6,7) = 1.
    // Total for x = (0, 3, 0, 0, 0, 0, 2, 0):
    // - Root 0: L1 = 3 + 2 = 5, Hamming = 2 + 1 = 3.
    // - Root 10: L1 = 3 + 2 = 5, Hamming = 1 + 1 = 2!
    // What about Root 9 (0..0, 2, -2)? On (6,7)=(2,0), L1 = 0 + 2 = 2, Hamming = 1 + 1 = 2, and 9 < 10!
    int32_t x_ham[8] = {0, 3, 0, 0, 0, 0, 2, 0};
    uint32_t d0 = 0, h0 = 0, d9 = 0, h9 = 0;
    root_cost(CANONICAL_E8_ORBIT_16, x_ham, 0, d0, h0);
    root_cost(CANONICAL_E8_ORBIT_16, x_ham, 9, d9, h9);
    assert(d0 == 5 && h0 == 3);
    assert(d9 == 5 && h9 == 2);
    assert(snap_e8_root(CANONICAL_E8_ORBIT_16, x_ham, 1) == 9);
    std::cout << "  • Test 2: Lexicographic (L1, Hamming, index) tie-breaking ... [PASS]\n";

    // Test 3: Zero vector snaps to root 0 (all roots have L1=4, Hamming=2, tie broken by r=0)
    int32_t x_zero[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    assert(snap_e8_root(CANONICAL_E8_ORBIT_16, x_zero, 1) == 0);
    std::cout << "  • Test 3: Origin tie-breaking to index 0 ... [PASS]\n";

    constexpr int ITERS = 100000;
    volatile uint8_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ITERS; ++i) {
        x_ham[0] = (i & 3) - 1;
        sink = sink + snap_e8_root(CANONICAL_E8_ORBIT_16, x_ham, 1);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    (void)sink;
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / ITERS;
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
