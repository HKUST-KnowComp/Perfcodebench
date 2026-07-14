#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    const uint32_t words = (key_space + 63) >> 6;
    std::vector<uint64_t> bits(words, 0);
    std::vector<uint32_t> touched;
    touched.reserve(keys.size());

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        touched.clear();
        for (uint32_t key : keys) {
            const uint32_t idx = key >> 6;
            const uint64_t mask = 1ULL << (key & 63);
            const uint64_t old = bits[idx];
            if (!(old & mask)) {
                bits[idx] = old | mask;
                touched.push_back(idx);
            }
        }
        count = 0;
        for (uint32_t w : touched) {
            count += __builtin_popcountll(bits[w]);
            bits[w] = 0;
        }
    }
    return count;
}