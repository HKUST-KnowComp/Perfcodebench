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
    const std::size_t n_probe = probe_ts.size();
    const std::size_t n_dim   = dim_ts.size();

    const uint32_t* __restrict d_ts    = dim_ts.data();
    const uint32_t* __restrict d_vals  = dim_values.data();
    const uint32_t* __restrict p_ts    = probe_ts.data();
    const uint32_t* __restrict p_w     = probe_weights.data();

    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::size_t k = 0;                 // cursor in dim arrays
        uint64_t local_sum = 0;

        for (std::size_t i = 0; i < n_probe; ++i) {
            const uint32_t cur_ts = p_ts[i];
            while (k < n_dim && d_ts[k] <= cur_ts) {
                ++k;
            }
            // k == 0  ⇒ nothing satisfies, but per problem statement this is impossible
            local_sum += static_cast<uint64_t>(d_vals[k - 1]) *
                         static_cast<uint64_t>(p_w[i]);
        }
        sum = local_sum;
    }
    return sum;
}