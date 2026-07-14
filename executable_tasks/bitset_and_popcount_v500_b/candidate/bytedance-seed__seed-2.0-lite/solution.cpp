#include "interface.h"

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    uint64_t count = 0;
    const std::size_t word_count = left.size();
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        for (std::size_t i = 0; i < word_count; ++i) {
            count += __builtin_popcountll(left[i] & right[i]);
        }
    }
    return count;
}