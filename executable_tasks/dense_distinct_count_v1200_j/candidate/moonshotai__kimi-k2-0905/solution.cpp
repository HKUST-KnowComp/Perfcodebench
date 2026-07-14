#include "interface.h"
#include <vector>
#include <cstdint>
#include <bit>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    const uint32_t n_words = (key_space + 63) >> 6;
    std::vector<uint64_t> bits(n_words, 0);
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(bits.begin(), bits.end(), 0);
        for (uint32_t k : keys) {
            const uint32_t idx = k >> 6;
            const uint32_t bit = k & 63;
            bits[idx] |= (1ULL << bit);
        }
        uint64_t pop = 0;
        for (uint64_t w : bits) pop += std::popcount(w);
        count = pop;
    }
    return count;
}