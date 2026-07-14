#include "interface.h"

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t pop = 0;
    for (int i = 0; i < iters; ++i) {
        uint64_t count = 0;
        for (const uint64_t word : words) {
            count += __builtin_popcountll(word);
        }
        pop = count;
    }
    return pop;
}