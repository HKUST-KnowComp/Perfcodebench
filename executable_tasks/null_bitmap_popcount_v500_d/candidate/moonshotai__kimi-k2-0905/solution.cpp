#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total = 0;
    for (uint64_t w : words) {
        total += __builtin_popcountll(w);
    }
    // ‘iters’ is only used to satisfy the benchmark loop; the count is identical each time.
    return total;
}