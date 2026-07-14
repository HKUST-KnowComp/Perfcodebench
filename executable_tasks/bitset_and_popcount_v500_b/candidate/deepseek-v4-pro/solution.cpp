#include "interface.h"
#include <cstdint>
#include <vector>
#include <bit>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters) {
    uint64_t count = 0;
    const std::size_t n = left.size();
    const uint64_t* l = left.data();
    const uint64_t* r = right.data();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t total = 0;
        for (std::size_t i = 0; i < n; ++i) {
            total += std::popcount(l[i] & r[i]);
        }
        count = total;
    }
    return count;
}