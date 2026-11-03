#include <cassert>
#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "solution.hpp"

int main() {
    std::cout << "=== Running 027 Grammar-Free Byte-Markov + CA 184/30 Stream Chunker Suite ===\n";

    // Test 1: Empty and short stream partition invariants
    auto empty_chunks = chunk_unformatted_byte_stream("");
    assert(empty_chunks.empty());

    std::string short_s(80, 'A');
    auto short_chunks = chunk_unformatted_byte_stream(short_s);
    assert(short_chunks.size() == 1);
    assert(short_chunks[0].start_byte == 0 && short_chunks[0].end_byte == 80 && short_chunks[0].chunk_idx == 1);
    std::cout << "  • Test 1: Empty & short stream boundary invariants ... [PASS]\n";

    // Test 2: Horizon cap splits a 1200-byte constant stream at 512-byte intervals
    std::string flat_s(1200, 'x');
    auto flat_chunks = chunk_unformatted_byte_stream(flat_s, nullptr);
    assert(flat_chunks.size() == 3);
    assert(flat_chunks[0].start_byte == 0 && flat_chunks[0].end_byte == 512 && flat_chunks[0].chunk_idx == 1);
    assert(flat_chunks[1].start_byte == 512 && flat_chunks[1].end_byte == 1024 && flat_chunks[1].chunk_idx == 2);
    assert(flat_chunks[2].start_byte == 1024 && flat_chunks[2].end_byte == 1200 && flat_chunks[2].chunk_idx == 3);
    std::cout << "  • Test 2: 512-byte horizon cap & exact partition coverage ... [PASS]\n";

    // Test 3: Phase spike triggers early segmentation (< 512 bytes) on Markov anomaly + high CA demarcs
    ByteMarkovModel markov{};
    std::string train_s(2000, 'a');
    markov.observe_stream(train_s);

    // Construct a 400-byte stream of 'a' with an abrupt phase spike around pos = 160
    std::string spike_s(400, 'a');
    // Inject high-demarc 8-byte pattern right before pos=160 and unexpected byte at pos=160
    const char* pattern = "\x55\xAA\x33\xCC\x0F\xF0\x69\x96Z";
    for (size_t k = 0; k < 9; ++k) {
        spike_s[152 + k] = pattern[k];
    }
    uint64_t win = 0;
    std::memcpy(&win, spike_s.data() + 152, 8);
    assert(count_demarcs(win) >= 28);

    auto spike_chunks = chunk_unformatted_byte_stream(spike_s, &markov);
    assert(spike_chunks.size() == 2);
    assert(spike_chunks[0].start_byte == 0 && spike_chunks[0].end_byte == 160);
    assert(spike_chunks[1].start_byte == 160 && spike_chunks[1].end_byte == 400);
    std::cout << "  • Test 3: Joint Markov surprisal spike + CA 184/30 demarcation split at pos=160 ... [PASS]\n";

    // Benchmark loop
    constexpr size_t ITERS = 50000;
    volatile size_t sink = 0;
    auto t0 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERS; ++i) {
        auto c = chunk_unformatted_byte_stream(spike_s, &markov);
        sink += c.size();
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    double ns_per_op = std::chrono::duration<double, std::nano>(t1 - t0).count() / static_cast<double>(ITERS);
    (void)sink;

    std::cout << "All 027 tests passed!\n";
    std::cout << "SCOREBOARD_JSON: {\"cycles_per_op\": " << ns_per_op << "}\n";
    return 0;
}
