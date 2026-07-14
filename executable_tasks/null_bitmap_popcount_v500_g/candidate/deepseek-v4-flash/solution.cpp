#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        for (const uint64_t& word : words) {
            count += __builtin_popcountll(word);
        }
    }
    return count;
}
