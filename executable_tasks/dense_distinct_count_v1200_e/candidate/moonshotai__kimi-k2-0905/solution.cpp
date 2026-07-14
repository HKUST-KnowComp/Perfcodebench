#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

inline uint64_t popcnt64(uint64_t x) {
    return static_cast<uint64_t>(__builtin_popcountll(x));
}

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    // key_space is guaranteed to be <= 2^32
    const uint32_t n_words = (key_space + 63) >> 6;
    // Allocate once and reuse the same buffer across iterations
    std::vector<uint64_t> bitset(n_words, 0);

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Reset bitmap in one memset instead of per-element clearing
        std::memset(bitset.data(), 0, n_words * sizeof(uint64_t));

        // Mark keys
        for (uint32_t key : keys) {
            const uint32_t word_idx = key >> 6;
            const uint64_t mask = uint64_t(1) << (key & 63);
            bitset[word_idx] |= mask;
        }

        // Count set bits
        uint64_t total = 0;
        for (uint32_t i = 0; i < n_words; ++i) {
            total += popcnt64(bitset[i]);
        }
        count = total;
    }
    return count;
}