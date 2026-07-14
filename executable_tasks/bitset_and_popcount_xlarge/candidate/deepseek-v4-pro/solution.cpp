#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    const std::size_t n = left.size();
    uint64_t final_count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t count = 0;
        for (std::size_t i = 0; i < n; ++i) {
            count += __builtin_popcountll(left[i] & right[i]);
        }
        final_count = count;
    }
    return final_count;
}