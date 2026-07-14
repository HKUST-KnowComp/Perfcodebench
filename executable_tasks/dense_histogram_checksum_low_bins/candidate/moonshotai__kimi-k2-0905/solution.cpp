#include "interface.h"

#include <cstdint>
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
    alignas(64) uint32_t counts[4096];
    const size_t binsz = (bins > 4096 ? 4096 : bins);
    const size_t bytes = binsz * sizeof(counts[0]);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(counts, 0, bytes);

        for (uint32_t v : values) {
            if (v < binsz) ++counts[v];
        }

        hash = 1469598103934665603ULL;
        for (size_t b = 0; b < binsz; ++b) {
            hash = mix(hash, counts[b]);
        }
    }

    return hash;
}