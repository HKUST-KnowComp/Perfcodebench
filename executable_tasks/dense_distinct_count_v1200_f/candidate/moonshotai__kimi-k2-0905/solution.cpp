#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    const uint32_t n_bits = key_space;
    const uint32_t n_words = (n_bits + 63) / 64;
    std::vector<uint64_t> bitset(n_words, 0);

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Clear bitset quickly: memset is vectorised
        if (n_words > 0) {
            std::memset(bitset.data(), 0, n_words * sizeof(uint64_t));
        }

        uint64_t distinct = 0;
        for (uint32_t key : keys) {
            const uint32_t word_idx = key >> 6;
            const uint64_t bit_mask = uint64_t(1) << (key & 63);
            uint64_t& word = bitset[word_idx];
            if ((word & bit_mask) == 0) {
                word |= bit_mask;
                ++distinct;
            }
        }
        count = distinct;
    }
    return count;
}