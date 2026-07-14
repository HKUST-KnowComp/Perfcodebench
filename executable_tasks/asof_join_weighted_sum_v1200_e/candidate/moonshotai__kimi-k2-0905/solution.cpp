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
    const std::size_t n_p = probe_ts.size();
    const std::size_t n_d = dim_ts.size();
    uint64_t sum = 0;

    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        std::size_t d = 0;                 // current dimension index
        uint32_t  cur_val = dim_values[0]; // value at dim_ts[d]

        for (std::size_t p = 0; p < n_p; ++p) {
            const uint32_t ts_p = probe_ts[p];
            // advance dimension pointer while next dim_ts is <= probe_ts
            while (d + 1 < n_d && dim_ts[d + 1] <= ts_p) {
                ++d;
                cur_val = dim_values[d];
            }
            sum += static_cast<uint64_t>(cur_val) *
                   static_cast<uint64_t>(probe_weights[p]);
        }
    }
    return sum;
}