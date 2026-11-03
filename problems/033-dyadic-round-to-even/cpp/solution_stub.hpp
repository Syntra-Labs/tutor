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

    static constexpr uint32_t ctz_u128(__uint128_t x) {
        if (x == 0) return 128;
        uint64_t l = static_cast<uint64_t>(x);
        if (l != 0) return static_cast<uint32_t>(std::countr_zero(l));
        return 64u + static_cast<uint32_t>(std::countr_zero(static_cast<uint64_t>(x >> 64)));
    }

    constexpr uint32_t bits() const {
        if (hi != 0) return 256u - clz_u128(hi);
        return 128u - clz_u128(lo);
    }

    constexpr U256 shl(uint32_t k) const {
        if (k == 0) return *this;
        if (k < 128) return { (hi << k) | (lo >> (128 - k)), lo << k };
        if (k < 256) return { lo << (k - 128), 0 };
        return {0, 0};
    }

    constexpr std::pair<U256, bool> shr_sticky(uint32_t k) const {
        if (k == 0) return {*this, false};
        if (k < 128) {
            __uint128_t mask = (static_cast<__uint128_t>(1) << k) - 1;
            return { {hi >> k, (lo >> k) | (hi << (128 - k))}, (lo & mask) != 0 };
        }
        if (k < 256) {
            uint32_t s = k - 128;
            __uint128_t mask = (s == 0) ? 0 : ((static_cast<__uint128_t>(1) << s) - 1);
            return { {0, hi >> s}, (lo != 0) || ((hi & mask) != 0) };
        }
        return { {0, 0}, !is_zero() };
    }

    static constexpr U256 mul_u128(__uint128_t a, __uint128_t b) {
        constexpr __uint128_t LOW64 = (~static_cast<__uint128_t>(0)) >> 64;
        __uint128_t a1 = a >> 64, a0 = a & LOW64;
        __uint128_t b1 = b >> 64, b0 = b & LOW64;
        __uint128_t p00 = a0 * b0, p01 = a0 * b1, p10 = a1 * b0, p11 = a1 * b1;
        __uint128_t mid = p01 + p10;
        __uint128_t c1 = (mid < p01) ? 1 : 0;
        __uint128_t mid_lo = mid << 64;
        __uint128_t lo = p00 + mid_lo;
        __uint128_t c2 = (lo < p00) ? 1 : 0;
        __uint128_t hi = p11 + (mid >> 64) + (c1 << 64) + c2;
        return {hi, lo};
    }
};

template <uint32_t P = 120>
struct Dy {
    static_assert(P >= 8 && P <= 120, "Dy precision P must be in 8..=120");
    __int128 m = 0;
    int32_t e = 0;

    constexpr bool operator==(const Dy& o) const = default;

    static constexpr Dy round(bool neg, U256 mag, int64_t exp, bool sticky) {
        // TODO: Round (-1)^neg * (mag + f) * 2^exp to P bits, nearest, ties to even,
        // and canonicalize so m is odd (or m = 0, e = 0)
        (void)neg;
        (void)mag;
        (void)exp;
        (void)sticky;
        return {};
    }

    static constexpr Dy from_i64(int64_t v) {
        __uint128_t abs_v = (v < 0) ? (static_cast<__uint128_t>(-(v + 1)) + 1) : static_cast<__uint128_t>(v);
        return round(v < 0, U256{0, abs_v}, 0, false);
    }

    constexpr Dy mul(Dy o) const {
        // TODO: Multiply two Dy<P> values via U256::mul_u128 and round once
        (void)o;
        return {};
    }
};
