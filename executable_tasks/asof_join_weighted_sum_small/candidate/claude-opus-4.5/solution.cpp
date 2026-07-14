#include "interface.h"

#include <cstdint>

#if defined(__GNUC__) || defined(__clang__)
#define PREFETCH(addr) __builtin_prefetch(addr, 0, 0)
#else
#define PREFETCH(addr)
#endif

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {

    const std::size_t probe_n = probe_ts.size();
    const std::size_t dim_n = dim_ts.size();

    if (probe_n == 0 || dim_n == 0) {
        return 0;
    }

    const uint32_t* __restrict__ p_ts = probe_ts.data();
    const uint32_t* __restrict__ p_wt = probe_weights.data();
    const uint32_t* __restrict__ d_ts = dim_ts.data();
    const uint32_t* __restrict__ d_val = dim_values.data();

    const std::size_t dim_last = dim_n - 1;

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t d = 0;

        std::size_t i = 0;
        const std::size_t unroll_end = (probe_n >= 4) ? (probe_n - 3) : 0;

        while (i < unroll_end) {
            PREFETCH(p_ts + i + 16);
            PREFETCH(p_wt + i + 16);

            // Element 0
            {
                uint32_t ts0 = p_ts[i];
                while (d < dim_last && d_ts[d + 1] <= ts0) ++d;
                sum += static_cast<uint64_t>(d_val[d]) * static_cast<uint64_t>(p_wt[i]);
            }
            // Element 1
            {
                uint32_t ts1 = p_ts[i + 1];
                while (d < dim_last && d_ts[d + 1] <= ts1) ++d;
                sum += static_cast<uint64_t>(d_val[d]) * static_cast<uint64_t>(p_wt[i + 1]);
            }
            // Element 2
            {
                uint32_t ts2 = p_ts[i + 2];
                while (d < dim_last && d_ts[d + 1] <= ts2) ++d;
                sum += static_cast<uint64_t>(d_val[d]) * static_cast<uint64_t>(p_wt[i + 2]);
            }
            // Element 3
            {
                uint32_t ts3 = p_ts[i + 3];
                while (d < dim_last && d_ts[d + 1] <= ts3) ++d;
                sum += static_cast<uint64_t>(d_val[d]) * static_cast<uint64_t>(p_wt[i + 3]);
            }
            i += 4;
        }

        // Scalar tail
        for (; i < probe_n; ++i) {
            uint32_t ts = p_ts[i];
            while (d < dim_last && d_ts[d + 1] <= ts) ++d;
            sum += static_cast<uint64_t>(d_val[d]) * static_cast<uint64_t>(p_wt[i]);
        }
    }

    return sum;
}
