#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters)
{
    const uint32_t * const b = boundaries.data();
    const uint32_t * const p = probes.data();
    const size_t B = boundaries.size();
    const size_t P = probes.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t b_idx = 0;
        const size_t P4 = P & ~size_t(3);
        size_t i = 0;

        // 4-unrolled branch-free scan
        for (; i < P4; i += 4) {
            uint32_t v0 = p[i];
            uint32_t v1 = p[i+1];
            uint32_t v2 = p[i+2];
            uint32_t v3 = p[i+3];

            while (b_idx < B && b[b_idx] <= v0) ++b_idx;
            uint64_t k0 = b_idx;
            size_t b1 = b_idx;
            while (b1 < B && b[b1] <= v1) ++b1;
            uint64_t k1 = b1;
            size_t b2 = b1;
            while (b2 < B && b[b2] <= v2) ++b2;
            uint64_t k2 = b2;
            size_t b3 = b2;
            while (b3 < B && b[b3] <= v3) ++b3;
            uint64_t k3 = b3;

            sum += k0 + k1 + k2 + k3;
            b_idx = b3;
        }
        // tail
        for (; i < P; ++i) {
            uint32_t v = p[i];
            while (b_idx < B && b[b_idx] <= v) ++b_idx;
            sum += b_idx;
        }
    }
    return sum;
}