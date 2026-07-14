#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t interval_lookup_sum(
    const std::vector<uint32_t>& starts,
    const std::vector<uint32_t>& ends,
    const std::vector<uint32_t>& payloads,
    const std::vector<uint32_t>& probes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const size_t n = starts.size();
    const size_t m = probes.size();
    uint64_t total = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        size_t i = 0;          // interval index
        uint64_t active = 0;   // running sum of payloads whose intervals cover the current probe

        for (size_t j = 0; j < m; ++j) {
            const uint32_t p = probes[j];
            // advance intervals whose start is <= p
            while (i < n && starts[i] <= p) {
                active += payloads[i];
                ++i;
            }
            // retract intervals whose end < p
            size_t k = 0;
            while (k < i && ends[k] < p) {
                active -= payloads[k];
                ++k;
            }
            // shrink the active window by moving the retracted prefix forward
            if (k > 0) {
                // shift remaining intervals left by k positions
                for (size_t t = 0; t < i - k; ++t) {
                    starts[t] = starts[t + k];
                    ends[t]   = ends[t + k];
                    payloads[t] = payloads[t + k];
                }
                i -= k;
            }
            sum += active * static_cast<uint64_t>(weights[j]);
        }
        total = sum;
    }
    return total;
}