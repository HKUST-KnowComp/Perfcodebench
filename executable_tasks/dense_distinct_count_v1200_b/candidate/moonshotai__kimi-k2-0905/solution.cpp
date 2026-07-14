#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    const uint32_t n_bits = key_space;
    const uint32_t n_words = (n_bits + 63) / 64;
    std::vector<uint64_t> bits(n_words, 0);
    uint64_t count = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // bulk clear: much faster than per-key clear
        std::memset(bits.data(), 0, n_words * sizeof(uint64_t));

        for (uint32_t k : keys) {
            const uint32_t word_idx = k >> 6;
            const uint64_t mask = 1ULL << (k & 63);
            bits[word_idx] |= mask;
        }

        uint64_t pop = 0;
        for (uint32_t i = 0; i < n_words; ++i) {
            pop += __builtin_popcountll(bits[i]);
        }
        count = pop;
    }
    return count;
}