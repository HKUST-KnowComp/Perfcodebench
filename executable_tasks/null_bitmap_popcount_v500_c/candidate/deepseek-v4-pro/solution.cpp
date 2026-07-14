#include "interface.h"

#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int /*iters*/) {
    uint64_t count = 0;
    for (uint64_t word : words) {
        count += __builtin_popcountll(word);
    }
    return count;
}
