#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters)
{
    const uint32_t* const __restrict b = boundaries.data();
    const uint32_t* const __restrict p = probes.data();
    const size_t b_size = boundaries.size();
    const size_t p_size = probes.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t b_idx = 0;
        for (size_t i = 0; i < p_size; ++i) {
            const uint32_t v = p[i];
            while (b_idx < b_size && b[b_idx] <= v) ++b_idx;
            sum += b_idx;
        }
    }
    return sum;
}