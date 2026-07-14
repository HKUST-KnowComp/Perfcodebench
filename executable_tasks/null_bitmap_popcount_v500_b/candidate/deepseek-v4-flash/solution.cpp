#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t count = 0;
    for (int i = 0; i < iters; ++i) {
        count = 0;
        for (std::size_t j = 0; j < words.size(); ++j) {
            count += __builtin_popcountll(words[j]);
        }
    }
    return count;
}
