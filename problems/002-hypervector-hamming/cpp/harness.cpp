#include <cassert>
#include <cmath>
#include <iostream>

#include "solution.hpp"

int main() {
    std::cout << "=== Running Hypervector Hamming Test Suite ===\n";

    Hypervec8192 v1{};
    Hypervec8192 v2{};

    // Identical
    assert(hamming_distance(v1, v2) == 0);
    assert(std::abs(cosine_similarity(v1, v2) - 1.0) < 1e-6);
    std::cout << "  • Test 1: Zero vectors identical ... [PASS]\n";

    // Inverse
    for (auto& w : v2.words) w = ~0ULL;
    assert(hamming_distance(v1, v2) == 8192);
    assert(std::abs(cosine_similarity(v1, v2) - (-1.0)) < 1e-6);
    std::cout << "  • Test 2: Inverted vector antipodal distance ... [PASS]\n";

    // Orthogonal (half bits set)
    for (size_t i = 0; i < 64; ++i) v2.words[i] = ~0ULL;
    for (size_t i = 64; i < 128; ++i) v2.words[i] = 0ULL;
    assert(hamming_distance(v1, v2) == 4096);
    assert(std::abs(cosine_similarity(v1, v2) - 0.0) < 1e-6);
    std::cout << "  • Test 3: Orthogonal vector cosine similarity == 0.0 ... [PASS]\n";

    std::cout << "All Hypervector Hamming tests passed successfully!\n";
    return 0;
}
