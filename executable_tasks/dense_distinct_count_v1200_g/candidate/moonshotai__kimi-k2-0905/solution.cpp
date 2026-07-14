#include "interface.h"

#include <vector>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys,
                        uint32_t key_space,
                        int iters) {
    // key_space is guaranteed ≤ 1'000'000
    constexpr uint32_t bits_per_word = 64U;
    const uint32_t words = (key_space + bits_per_word - 1U) / bits_per_word;
    std::vector<uint64_t> bitset(words, 0ULL);

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(bitset.data(), 0, words * sizeof(uint64_t));

        for (uint32_t key : keys) {
            const uint32_t idx   = key / bits_per_word;
            const uint32_t shift = key % bits_per_word;
            bitset[idx] |= (1ULL << shift);
        }

        uint64_t pop = 0;
        for (uint64_t word : bitset) {
            pop += __builtin_popcountll(word);
        }
        count = pop;
    }
    return count;
}