#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    const std::size_t n_dim   = dim_ts.size();
    const std::size_t n_probe = probe_ts.size();

    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::size_t k = 0;n        uint64_t acc = 0;

        // unroll 4× to reduce loop overhead and help auto-vectorization
        std::size_t i = 0;
        for (; i + 4 <= n_probe; i += 4) {
            const uint32_t t0 = probe_ts[i + 0];
            const uint32_t t1 = probe_ts[i + 1];
            const uint32_t t2 = probe_ts[i + 2];
            const uint32_t t3 = probe_ts[i + 3];

            while (k < n_dim && dim_ts[k] <= t0) ++k;
            const uint64_t v0 = dim_values[k - 1];
            acc += v0 * probe_weights[i + 0];

            while (k < n_dim && dim_ts[k] <= t1) ++k;
            const uint64_t v1 = dim_values[k - 1];
            acc += v1 * probe_weights[i + 1];

            while (k < n_dim && dim_ts[k] <= t2) ++k;
            const uint64_t v2 = dim_values[k - 1];
            acc += v2 * probe_weights[i + 2];

            while (k < n_dim && dim_ts[k] <= t3) ++k;
            const uint64_t v3 = dim_values[k - 1];
            acc += v3 * probe_weights[i + 3];
        }

        // tail
        for (; i < n_probe; ++i) {
            const uint32_t t = probe_ts[i];
            while (k < n_dim && dim_ts[k] <= t) ++k;
            acc += static_cast<uint64_t>(dim_values[k - 1]) *
                   static_cast<uint64_t>(probe_weights[i]);
        }

        total = acc;
    }

    return total;
}