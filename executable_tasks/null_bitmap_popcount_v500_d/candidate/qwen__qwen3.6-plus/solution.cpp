#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t result = 0;
    for (int i = 0; i < iters; ++i) {
        uint64_t count = 0;
        for (uint64_t w : words) {
            count += __builtin_popcountll(w);
        }
        result = count;
    }
    return result;
}