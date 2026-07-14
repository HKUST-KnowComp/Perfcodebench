#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    uint64_t count = 0;
    const size_t n = left.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (size_t i = 0; i < n; ++i) {
            sum += __builtin_popcountll(left[i] & right[i]);
        }
        count = sum;
    }
    return count;
}