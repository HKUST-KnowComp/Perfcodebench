#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (std::size_t i = 0; i < words.size(); ++i) {
            sum += static_cast<uint64_t>(__builtin_popcountll(words[i]));
        }
        total = sum;  // last iteration's result is returned
    }
    return total;
}