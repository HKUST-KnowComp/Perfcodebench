#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    if (iters <= 0) {
        return 0;
    }

    uint64_t count = 0;
    const std::size_t n = left.size();
    const uint64_t* l_ptr = left.data();
    const uint64_t* r_ptr = right.data();

    std::size_t i = 0;
    // Unroll loop to maximize instruction-level parallelism
    for (; i + 7 < n; i += 8) {
        count += __builtin_popcountll(l_ptr[i] & r_ptr[i]);
        count += __builtin_popcountll(l_ptr[i+1] & r_ptr[i+1]);
        count += __builtin_popcountll(l_ptr[i+2] & r_ptr[i+2]);
        count += __builtin_popcountll(l_ptr[i+3] & r_ptr[i+3]);
        count += __builtin_popcountll(l_ptr[i+4] & r_ptr[i+4]);
        count += __builtin_popcountll(l_ptr[i+5] & r_ptr[i+5]);
        count += __builtin_popcountll(l_ptr[i+6] & r_ptr[i+6]);
        count += __builtin_popcountll(l_ptr[i+7] & r_ptr[i+7]);
    }

    // Handle remaining elements
    for (; i < n; ++i) {
        count += __builtin_popcountll(l_ptr[i] & r_ptr[i]);
    }

    return count;
}
