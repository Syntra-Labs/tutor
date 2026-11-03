#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <vector>

inline constexpr std::array<uint64_t, 15> FIRST_15_PRIMES = {
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47
};

constexpr inline uint64_t nth_prime(size_t k) {
    // TODO: Return k-th prime (using FIRST_15_PRIMES for k < 15, or trial division for larger k)
    (void)k;
    return 2;
}

inline uint64_t encode_prime_set(const std::vector<size_t>& token_ids) {
    // TODO: Multiply unique primes p_k for k in token_ids (avoiding duplicate factors via std::gcd or bitmask)
    (void)token_ids;
    return 1;
}

inline std::vector<size_t> intersect_prime_sets(uint64_t prod_a, uint64_t prod_b) {
    // TODO: Compute g = std::gcd(prod_a, prod_b) and return sorted token IDs k whose p_k divides g
    (void)prod_a;
    (void)prod_b;
    return {};
}
