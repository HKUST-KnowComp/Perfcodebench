#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t count = 0;
    for (auto w : words) {
        count += __builtin_popcountll(w);
    }
    return count;
}
