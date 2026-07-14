#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters) {

    const std::size_t n_intervals = starts.size();
    const std::size_t n_probes    = probes.size();

    const uint32_t* __restrict s = starts.data();
    const uint32_t* __restrict e = ends.data();
    const uint32_t* __restrict p = payloads.data();
    const uint32_t* __restrict q = probes.data();
    const uint32_t* __restrict w = weights.data();

    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;

        std::size_t i = 0;          // interval index
        std::size_t j = 0;          // probe index

        while (i < n_intervals && j < n_probes) {
            if (q[j] < s[i]) {
                ++j;                // probe before current interval, skip it
            } else if (q[j] > e[i]) {
                ++i;                // probe after current interval, skip interval
            } else {
                // probe falls in interval i
                sum += static_cast<uint64_t>(p[i]) * static_cast<uint64_t>(w[j]);
                ++j;
            }
        }
    }
    return sum;
}