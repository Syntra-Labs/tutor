#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 066 Ternary Merkle Object Store Test Suite ===\n";

    // Test 1: Exact byte-for-byte rebuild(fold(b)) == b on non-multiple-of-32 payload
    {
        TernaryObjectStore store;
        std::vector<uint8_t> data(605);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = static_cast<uint8_t>((i * 131 + 17) & 0xFF);
        }
        Manifest m = store.fold(data);
        assert(m.byte_len == 605);
        assert(m.root_id != 0);
        std::vector<uint8_t> rebuilt = store.rebuild(m);
        assert(rebuilt == data);
        std::cout << "  • Test 1: Exact round-trip rebuild(fold(b)) == b (605 bytes) ... [PASS]\n";
    }

    // Test 2: Deduplication of repeated 7,776-byte (243 x 32B = 3^5 leaves) object into 6 unique records
    {
        TernaryObjectStore store;
        std::vector<uint8_t> repeated(243 * 32, 0x42);
        Manifest m = store.fold(repeated);
        assert(store.unique_record_count() == 6); // scales 0, 1, 2, 3, 4, 5
        assert(store.rebuild(m) == repeated);
        std::cout << "  • Test 2: 243 identical leaves deduplicated to 6 records ... [PASS]\n";
    }

    // Test 3: Selective range read [start, end) touches O(log_3 N) nodes
    {
        TernaryObjectStore store;
        std::vector<uint8_t> data(243 * 32);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = static_cast<uint8_t>((i * 73 + (i >> 5)) & 0xFF);
        }
        Manifest m = store.fold(data);

        size_t visited = 0;
        size_t start = 50;
        size_t end = 73; // 23-byte range spanning leaf 1 (32..64) and leaf 2 (64..96)
        std::vector<uint8_t> slice = store.range(m, start, end, &visited);
        assert(slice.size() == 23);
        for (size_t i = 0; i < 23; ++i) {
            assert(slice[i] == data[start + i]);
        }
        // Full tree has 364 nodes, but a 2-leaf range read in a depth-5 tree visits <= 12 nodes!
        assert(visited > 0 && visited <= 12);
        std::cout << "  • Test 3: Selective 23-byte range read visited " << visited << " nodes ... [PASS]\n";
    }

    // Performance benchmark
    TernaryObjectStore bench_store;
    std::vector<uint8_t> bench_data(81 * 32);
    for (size_t i = 0; i < bench_data.size(); ++i) bench_data[i] = static_cast<uint8_t>(i & 0xFF);
    Manifest bm = bench_store.fold(bench_data);

    constexpr size_t ITERS = 20000;
    uint64_t checksum = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        size_t offset = (i * 19) % 2000;
        auto s = bench_store.range(bm, offset, offset + 23, nullptr);
        if (!s.empty()) checksum += s[0];
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    if (checksum == 0xFFFFFFFFFFFFULL) std::cout << "";

    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
