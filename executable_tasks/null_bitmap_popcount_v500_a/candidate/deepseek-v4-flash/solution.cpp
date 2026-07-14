#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    (void)iters;
    uint64_t count = 0;
    for (uint64_t w : words) {
        count += __builtin_popcountll(w);
    }
    return count;
}
