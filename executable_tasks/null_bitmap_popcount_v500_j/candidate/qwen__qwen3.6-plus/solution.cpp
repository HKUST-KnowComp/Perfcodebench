#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t count = 0;
    const uint64_t* data = words.data();
    const size_t n = words.size();
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        for (size_t i = 0; i < n; ++i) {
            count += __builtin_popcountll(data[i]);
        }
    }
    return count;
}