#include "interface.h"
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    if (iters <= 0) return 0;
    uint64_t count = 0;
    for (uint64_t w : words) {
        count += __builtin_popcountll(w);
    }
    return count;
}
