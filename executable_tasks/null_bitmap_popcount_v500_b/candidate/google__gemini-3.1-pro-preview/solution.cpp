#include "interface.h"
#include <vector>

uint64_t bitmap_popcount(const std::vector<uint64_t>& words, int iters) {
    if (iters <= 0) return 0;
    
    uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;
    const uint64_t* ptr = words.data();
    std::size_t size = words.size();
    std::size_t i = 0;
    
    for (; i + 3 < size; i += 4) {
        c0 += __builtin_popcountll(ptr[i]);
        c1 += __builtin_popcountll(ptr[i+1]);
        c2 += __builtin_popcountll(ptr[i+2]);
        c3 += __builtin_popcountll(ptr[i+3]);
    }
    for (; i < size; ++i) {
        c0 += __builtin_popcountll(ptr[i]);
    }
    
    return c0 + c1 + c2 + c3;
}
