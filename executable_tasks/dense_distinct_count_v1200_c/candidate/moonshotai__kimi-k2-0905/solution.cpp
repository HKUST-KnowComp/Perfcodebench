#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    const std::size_t n_bits = key_space;
    const std::size_t n_words = (n_bits + 63) / 64;
    static thread_local std::vector<uint64_t> bitset(n_words, 0);

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // reset bitset in bulk
        std::memset(bitset.data(), 0, n_words * sizeof(uint64_t));
        uint64_t distinct = 0;

        for (uint32_t key : keys) {
            const std::size_t word_idx = key >> 6;
            const uint64_t mask = uint64_t(1) << (key & 63);
            const uint64_t prev = bitset[word_idx];
            bitset[word_idx] |= mask;
            distinct += (prev != bitset[word_idx]);   // 1 if new distinct key
        }
        count = distinct;
    }
    return count;
}