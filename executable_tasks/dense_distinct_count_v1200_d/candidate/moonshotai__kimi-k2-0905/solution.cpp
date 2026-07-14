#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
    const uint32_t n_bits = key_space;
    const uint32_t n_words = (n_bits + 63) >> 6;
    std::vector<uint64_t> bits(n_words, 0);
    std::vector<uint32_t> touched;
    touched.reserve(keys.size());

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        if (iter == 0) {
            for (uint32_t key : keys) {
                const uint32_t w = key >> 6;
                const uint64_t m = 1ULL << (key & 63);
                const uint64_t old = bits[w];
                bits[w] = old | m;
                if (!(old & m)) {
                    touched.push_back(w);
                }
            }
        } else {
            for (uint32_t w : touched) {
                bits[w] = 0;
            }
            touched.clear();
            for (uint32_t key : keys) {
                const uint32_t w = key >> 6;
                const uint64_t m = 1ULL << (key & 63);
                const uint64_t old = bits[w];
                bits[w] = old | m;
                if (!(old & m)) {
                    touched.push_back(w);
                }
            }
        }
        count = static_cast<uint64_t>(touched.size());
    }
    return count;
}