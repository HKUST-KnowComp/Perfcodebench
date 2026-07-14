#include "interface.h"

#include <algorithm>
#include <omp.h>
#include <vector>
#include <cstring>

namespace {

int num_threads() {
    int t = omp_get_max_threads();
    return t > 8 ? 8 : (t < 1 ? 1 : t);
}

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : data) {
        hash ^= static_cast<uint64_t>(v);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint8_t>& input, int bins, int iters) {
    const std::size_t n = input.size();
    const int nthr = num_threads();
    omp_set_num_threads(nthr);

    std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(counts.begin(), counts.end(), 0u);

#pragma omp parallel
        {
            alignas(64) uint32_t local[256] = {0};

#pragma omp for schedule(static) nowait
            for (std::size_t i = 0; i < n; ++i) {
                local[input[i]]++;
            }

#pragma omp critical
            {
                for (int b = 0; b < 256; ++b) {
                    counts[b] += local[b];
                }
            }
        }
    }

    return checksum_u32(counts);
}