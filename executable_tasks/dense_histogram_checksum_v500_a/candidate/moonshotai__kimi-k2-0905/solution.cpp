#include "interface.h"
#include <cstring>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values,
                            uint32_t bins, int iters) {
    if (bins == 0) return 0;
    alignas(64) uint32_t cnt[65536];           // 256 KiB, fits L2 on most CPUs
    const size_t n = values.size();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        memset(cnt, 0, bins * sizeof(uint32_t));

        for (size_t i = 0; i < n; ++i) {
            const uint32_t v = values[i];
            if (v < bins) ++cnt[v];
        }

        uint64_t h = 1469598103934665603ULL;
        for (uint32_t b = 0; b < bins; ++b) {
            h = mix(h, cnt[b]);
        }
        hash = h;
    }
    return hash;
}