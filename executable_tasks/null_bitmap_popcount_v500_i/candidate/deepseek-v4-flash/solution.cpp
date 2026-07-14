#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t total = 0;
    for (std::size_t i = 0; i < words.size(); ++i) {
        total += __builtin_popcountll(words[i]);
    }
    return total * static_cast<uint64_t>(iters);
}
