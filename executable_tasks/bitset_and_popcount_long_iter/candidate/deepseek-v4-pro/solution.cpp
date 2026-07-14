#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    const std::size_t n = left.size();
    if (n == 0 || iters <= 0) [[unlikely]] {
        return 0;
    }
    // Precompute the bitwise AND once to avoid repeating it in every iteration.
    std::vector<uint64_t> ands(n);
    for (std::size_t i = 0; i < n; ++i) {
        ands[i] = left[i] & right[i];
    }

    uint64_t count = 0;
    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        for (std::size_t i = 0; i < n; ++i) {
            count += __builtin_popcountll(ands[i]);
        }
    }
    return count;
}