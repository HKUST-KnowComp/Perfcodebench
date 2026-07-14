#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    const size_t n = left.size();
    const uint64_t* l = left.data();
    const uint64_t* r = right.data();
    
    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t iter_count = 0;
        for (size_t i = 0; i < n; ++i) {
            iter_count += __builtin_popcountll(l[i] & r[i]);
        }
        count = iter_count;
    }
    return count;
}