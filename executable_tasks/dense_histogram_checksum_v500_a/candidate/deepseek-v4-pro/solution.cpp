#include "interface.h"

#include <algorithm>
#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t kInitialHash = 1469598103934665603ULL;
constexpr uint64_t kMixMultiplier = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= kMixMultiplier;
    return hash;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
    std::vector<uint32_t> counts(bins);
    uint64_t hash = 0;

    const uint32_t* vals_data = values.data();
    const size_t vals_size = values.size();
    const uint32_t bins_local = bins;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(counts.begin(), counts.end(), 0U);

        for (size_t i = 0; i < vals_size; ++i) {
            uint32_t v = vals_data[i];
            if (v < bins_local) {
                ++counts[v];
            }
        }

        hash = kInitialHash;
        const uint32_t* cnt_data = counts.data();
        for (uint32_t b = 0; b < bins_local; ++b) {
            hash = mix(hash, cnt_data[b]);
        }
    }

    return hash;
}