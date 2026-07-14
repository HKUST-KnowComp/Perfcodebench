#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t range_bucket_sum(
    const std::vector<uint32_t>& boundaries,
    const std::vector<uint32_t>& probes,
    int iters) {
    uint64_t sum = 0;
    const size_t n = boundaries.size();
    for (int iter = 0; iter < iters; ++iter) {
        sum = 0;
        size_t idx = 0; // bucket index for current probe
        for (uint32_t probe : probes) {
            // advance idx while boundary <= probe
            while (idx < n && boundaries[idx] <= probe) {
                ++idx;
            }
            sum += static_cast<uint64_t>(idx);
        }
    }
    return sum;
}
