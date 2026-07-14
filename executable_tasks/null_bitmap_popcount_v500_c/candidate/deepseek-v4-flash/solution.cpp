#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t count = 0;
    for (int i = 0; i < iters; ++i) {
        uint64_t iter_count = 0;
        for (uint64_t w : words) {
            iter_count += __builtin_popcountll(w);
        }
        count = iter_count;
    }
    return count;
}
