#include "interface.h"
#include <cstdint>
#include <vector>
#include <algorithm>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    
    const size_t n = std::min(left.size(), right.size());
    const uint64_t* l = left.data();
    const uint64_t* r = right.data();
    
    uint64_t count = 0;
    for (size_t i = 0; i < n; ++i) {
        count += __builtin_popcountll(l[i] & r[i]);
    }
    
    (void)iters;
    return count;
}