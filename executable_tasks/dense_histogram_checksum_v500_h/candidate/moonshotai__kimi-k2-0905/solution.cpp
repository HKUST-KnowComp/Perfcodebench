#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) noexcept {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values,
                            uint32_t bins, int iters) {
    if (bins == 0) return 1469598103934665603ULL;

    std::vector<uint32_t> cnt(bins, 0);
    const uint32_t mask = bins - 1;               // works when bins is power of 2
    const bool pow2 = (bins & mask) == 0;

    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(cnt.begin(), cnt.end(), 0);

        for (uint32_t v : values) {
            uint32_t idx = pow2 ? (v & mask) : (v < bins ? v : bins - 1);
            ++cnt[idx];
        }

        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < bins; ++b) {
            hash = mix(hash, cnt[b]);
        }
    }
    return hash;
}