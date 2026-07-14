#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t bitset_and_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right,
    int iters)
{
    const std::size_t n = left.size();
    std::vector<uint64_t> and_buf(n);
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // 1. compute word-wise AND
        for (std::size_t i = 0; i < n; ++i) {
            and_buf[i] = left[i] & right[i];
        }
        // 2. popcount reduction
        uint64_t cnt = 0;
        for (std::size_t i = 0; i < n; ++i) {
            cnt += __builtin_popcountll(and_buf[i]);
        }
        total = cnt;   // keep final iteration's result
    }
    return total;
}