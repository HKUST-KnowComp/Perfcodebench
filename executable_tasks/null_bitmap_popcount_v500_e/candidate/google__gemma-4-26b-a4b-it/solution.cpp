#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_iter_count = 0;
        for (uint64_t word : words) {
            current_iter_count += static_cast<uint64_t>(__builtin_popcountll(word));
        }
        total_count = current_iter_count;
    }
    return total_count;
}