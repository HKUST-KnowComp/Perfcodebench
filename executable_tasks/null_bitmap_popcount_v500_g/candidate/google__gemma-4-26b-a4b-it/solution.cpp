#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total_count = 0;
    const size_t n = words.size();
    const uint64_t* data = words.data();

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_count = 0;
        for (size_t i = 0; i < n; ++i) {
            current_iter_count += static_cast<uint64_t>(__builtin_popcountll(data[i]));
        }
        total_count = current_iter_count;
    }
    return total_count;
}