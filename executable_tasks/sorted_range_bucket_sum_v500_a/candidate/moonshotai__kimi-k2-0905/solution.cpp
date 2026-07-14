#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters)
{
    const uint32_t* const b = boundaries.data();
    const uint32_t* const p = probes.data();
    const size_t B = boundaries.size();
    const size_t P = probes.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        size_t bidx = 0;
        for (size_t i = 0; i < P; ++i) {
            const uint32_t v = p[i];
            while (bidx < B && b[bidx] <= v) ++bidx;
            local += bidx;
        }
        sum = local;
    }
    return sum;
}