#include "interface.h"
#include <cstddef>
#include <cstdint>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    const uint32_t* const dts  = dim_ts.data();
    const uint32_t* const dval = dim_values.data();
    const uint32_t* const pts  = probe_ts.data();
    const uint32_t* const pw   = probe_weights.data();

    const std::size_t dim_n = dim_ts.size();
    const std::size_t probe_n = probe_ts.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t dim_idx = 0;

        // Peel first iteration to establish dim_idx >= 0
        if (probe_n > 0) {
            while (dim_idx + 1 < dim_n && dts[dim_idx + 1] <= pts[0])
                ++dim_idx;
            sum += static_cast<uint64_t>(dval[dim_idx]) * static_cast<uint64_t>(pw[0]);
        }

        // Main loop with 4-way unroll
        std::size_t i = 1;
        for (; i + 3 < probe_n; i += 4) {
            const uint32_t t0 = pts[i];
            const uint32_t t1 = pts[i + 1];
            const uint32_t t2 = pts[i + 2];
            const uint32_t t3 = pts[i + 3];

            while (dim_idx + 1 < dim_n && dts[dim_idx + 1] <= t0) ++dim_idx;
            const uint64_t v0 = dval[dim_idx];
            while (dim_idx + 1 < dim_n && dts[dim_idx + 1] <= t1) ++dim_idx;
            const uint64_t v1 = dval[dim_idx];
            while (dim_idx + 1 < dim_n && dts[dim_idx + 1] <= t2) ++dim_idx;
            const uint64_t v2 = dval[dim_idx];
            while (dim_idx + 1 < dim_n && dts[dim_idx + 1] <= t3) ++dim_idx;
            const uint64_t v3 = dval[dim_idx];

            sum += v0 * pw[i];
            sum += v1 * pw[i + 1];
            sum += v2 * pw[i + 2];
            sum += v3 * pw[i + 3];
        }

        // Tail
        for (; i < probe_n; ++i) {
            const uint32_t t = pts[i];
            while (dim_idx + 1 < dim_n && dts[dim_idx + 1] <= t)
                ++dim_idx;
            sum += static_cast<uint64_t>(dval[dim_idx]) * static_cast<uint64_t>(pw[i]);
        }
    }
    return sum;
}