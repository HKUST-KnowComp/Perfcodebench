#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t asof_join_sum(
    const std::vector<uint32_t>& __restrict__ dim_ts,
    const std::vector<uint32_t>& __restrict__ dim_values,
    const std::vector<uint32_t>& __restrict__ probe_ts,
    const std::vector<uint32_t>& __restrict__ probe_weights,
    int iters)
{
    const uint32_t * __restrict__ dt  = dim_ts.data();
    const uint32_t * __restrict__ dv  = dim_values.data();
    const uint32_t * __restrict__ pt  = probe_ts.data();
    const uint32_t * __restrict__ pw  = probe_weights.data();

    const size_t dim_n = dim_ts.size();
    const size_t probe_n = probe_ts.size();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        for (size_t i = 0; i < probe_n; ++i) {
            const uint32_t key = pt[i];
            size_t lo = 0;
            size_t hi = dim_n;
            while (lo < hi) {
                size_t mid = (lo + hi) >> 1;
                if (dt[mid] > key) hi = mid;
                else               lo = mid + 1;
            }
            const size_t idx = lo - 1;
            sum += static_cast<uint64_t>(dv[idx]) * static_cast<uint64_t>(pw[i]);
        }
    }
    return sum;
}