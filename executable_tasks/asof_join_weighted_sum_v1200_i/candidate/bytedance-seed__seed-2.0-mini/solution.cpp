#include "interface.h"

uint64_t asof_join_sum(
    const std::vector<uint32_t>& dim_ts,
    const std::vector<uint32_t>& dim_values,
    const std::vector<uint32_t>& probe_ts,
    const std::vector<uint32_t>& probe_weights,
    int iters) {
    uint64_t total_sum = 0;
    const size_t num_probes = probe_ts.size();
    const size_t dim_size = dim_ts.size();

    std::vector<size_t> ks(num_probes);
    if (num_probes > 0 && dim_size > 0) {
        size_t k = 0;
        while (k < dim_size - 1 && dim_ts[k + 1] <= probe_ts[0]) {
            k++;
        }
        ks[0] = k;

        for (size_t i = 1; i < num_probes; ++i) {
            const uint32_t t = probe_ts[i];
            while (k < dim_size - 1 && dim_ts[k + 1] <= t) {
                k++;
            }
            ks[i] = k;
        }
    }

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        for (size_t i = 0; i < num_probes; ++i) {
            sum += static_cast<uint64_t>(dim_values[ks[i]]) * static_cast<uint64_t>(probe_weights[i]);
        }
        total_sum = sum;
    }

    return total_sum;
}