#include "interface.h"
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_count = 0;
        for (std::size_t i = 0; i < words.size(); ++i) {
            iter_count += __builtin_popcountll(words[i]);
        }
        count = iter_count;
    }
    return count;
}