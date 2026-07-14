#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters)
{
    const size_t P = probe_ts.size();
    const size_t D = dim_ts.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t d = 0;                       // current position in dim arrays
        for (size_t p = 0; p < P; ++p) {
            const uint32_t t_probe = probe_ts[p];
            // advance d while the next dim timestamp is still <= probe timestamp
            while (d + 1 < D && dim_ts[d + 1] <= t_probe)
                ++d;
            sum += static_cast<uint64_t>(dim_values[d]) *
                   static_cast<uint64_t>(probe_weights[p]);
        }
    }
    return sum;
}