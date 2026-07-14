#include "interface.h"
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t final_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t count = 0;
        for (uint64_t word : words) {
            count += __builtin_popcountll(word);
        }
        final_count = count;
    }
    return final_count;
}