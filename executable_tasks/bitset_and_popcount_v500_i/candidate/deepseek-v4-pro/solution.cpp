#include "interface.h"
#include <cstddef>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    if (iters <= 0) return 0;
    uint64_t total = 0;
    for (std::size_t i = 0; i < left.size(); ++i) {
        total += __builtin_popcountll(left[i] & right[i]);
    }
    return total;
}