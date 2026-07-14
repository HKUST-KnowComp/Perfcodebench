#include "interface.h"

#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total = 0;
    for (const auto& word : words) {
        total += __builtin_popcountll(word);
    }
    return total;
}
