#include "interface.h"

#include <cstddef>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    if (iters == 0) return 0;

    const std::size_t P = probe_ts.size();
    const std::size_t D = dim_ts.size();
    if (P == 0) return 0;

    uint64_t single_sum = 0;
    std::size_t d = 0;
    for (std::size_t p = 0; p < P; ++p) {
        // Advance d while the next dimension timestamp is not greater than probe
        while (d + 1 < D && dim_ts[d + 1] <= probe_ts[p]) {
            ++d;
        }
        single_sum += static_cast<uint64_t>(dim_values[d]) *
                      static_cast<uint64_t>(probe_weights[p]);
    }
    return single_sum * static_cast<uint64_t>(iters);
}