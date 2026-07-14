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
                            uint32_t bins,
                            int iters) {
    std::vector<uint32_t> hist(bins, 0u);
    const uint32_t mask = bins - 1u;          // works when bins is power-of-two
    const bool pow2 = (bins & mask) == 0u;    // quick check

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // clear histogram (fast memset for trivial type)
        std::fill(hist.begin(), hist.end(), 0u);

        // accumulate counts
        for (uint32_t v : values) {
            uint32_t b = (pow2 ? (v & mask) : (v % bins));
            ++hist[b];
        }

        // compute checksum
        hash = 1469598103934665603ULL;
        for (uint32_t b = 0; b < bins; ++b) {
            hash = mix(hash, hist[b]);
        }
    }
    return hash;
}