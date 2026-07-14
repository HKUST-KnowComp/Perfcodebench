#include "interface.h"
#include <vector>
#include <cstring>

namespace {
inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values,
                            uint32_t bins,
                            int iters)
{
    std::vector<uint32_t> cnt(bins, 0u);
    const uint32_t* const data = values.data();
    const size_t n = values.size();

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(cnt.data(), 0, bins * sizeof(uint32_t));
        for (size_t i = 0; i < n; ++i) {
            const uint32_t idx = data[i];
            ++cnt[idx];
        }
        const uint32_t* c = cnt.data();
        for (uint32_t b = 0; b < bins; ++b) {
            hash = mix(hash, c[b]);
        }
    }
    return hash;
}