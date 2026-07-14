#include "interface.h"
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_sum = 0;
        for (uint64_t word : words) {
            current_sum += __builtin_popcountll(word);
        }
        count = current_sum;
    }
    return count;
}