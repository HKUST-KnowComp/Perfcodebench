#include "interface.h"
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    uint64_t count = 0;
    const uint64_t* data = words.data();
    const size_t n = words.size();
    
    for (int i = 0; i < iters; ++i) {
        uint64_t current = 0;
        for (size_t j = 0; j < n; ++j) {
            current += __builtin_popcountll(data[j]);
        }
        count = current;
    }
    return count;
}