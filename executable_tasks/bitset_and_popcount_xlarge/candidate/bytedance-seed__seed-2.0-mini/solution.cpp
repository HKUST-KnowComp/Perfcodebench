#include "interface.h"
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    uint64_t count = 0;
    const std::size_t word_count = left.size();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t total = 0;
        for (std::size_t i = 0; i < word_count; ++i) {
            const uint64_t combined = left[i] & right[i];
            total += __builtin_popcountll(combined);
        }
        count = total;
    }
    return count;
}