#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    uint64_t count = 0;
    const size_t n = left.size();
    const uint64_t* l_data = left.data();
    const uint64_t* r_data = right.data();

    for (int iter = 0; iter < iters; ++iter) {
        count = 0;
        for (size_t i = 0; i < n; ++i) {
            count += __builtin_popcountll(l_data[i] & r_data[i]);
        }
    }
    return count;
}
