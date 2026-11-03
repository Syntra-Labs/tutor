#pragma once

#include <bit>
#include <cstdint>
#include <utility>

struct U256 {
    __uint128_t hi = 0;
    __uint128_t lo = 0;

    constexpr bool operator==(const U256& o) const = default;

    constexpr bool is_zero() const { return hi == 0 && lo == 0; }

    static constexpr uint32_t clz_u128(__uint128_t x) {
        if (x == 0) return 128;
        uint64_t h = static_cast<uint64_t>(x >> 64);
        if (h != 0) return static_cast<uint32_t>(std::countl_zero(h));
        return 64u + static_cast<uint32_t>(std::countl_zero(static_cast<uint64_t>(x)));
    }

    constexpr uint32_t bits() const {
        // TODO: Return bit length in 0..=256
        return 0;
    }

    constexpr U256 shl(uint32_t k) const {
        // TODO: Shift left by k bits
        (void)k;
        return {};
    }

    constexpr std::pair<U256, bool> shr_sticky(uint32_t k) const {
        // TODO: Shift right by k bits and return (shifted, any_shifted_out_bit_was_one)
        (void)k;
        return {{}, false};
    }

    constexpr U256 add(U256 o) const {
        // TODO: 256-bit addition with carry
        (void)o;
        return {};
    }

    constexpr U256 sub(U256 o) const {
        // TODO: 256-bit subtraction with borrow
        (void)o;
        return {};
    }

    static constexpr U256 mul_u128(__uint128_t a, __uint128_t b) {
        // TODO: Exact 128x128 -> 256-bit unsigned multiplication
        (void)a;
        (void)b;
        return {};
    }
};
