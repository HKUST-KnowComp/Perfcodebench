#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    if (iters <= 0) return 0;
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t count = 0;
        for (std::size_t i = 0; i < words.size(); ++i) {
            count += __builtin_popcountll(words[i]);
        }
        total = count;
    }
    return total;
}