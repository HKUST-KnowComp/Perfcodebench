#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total_count = 0;
    size_t n = words.size();
    const uint64_t* data = words.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_count = 0;
        for (size_t i = 0; i < n; ++i) {
            current_iter_count += __builtin_popcountll(data[i]);
        }
        total_count = current_iter_count;
    }
    return total_count;
}