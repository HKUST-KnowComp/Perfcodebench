#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters)
{
    const size_t n = values.size();
    std::vector<uint32_t> partitioned;
    partitioned.reserve(n);

    // one stable partition up-front
    for (size_t i = 0; i < n; ++i) if (flags[i]) partitioned.push_back(values[i]);
    const size_t split = partitioned.size();
    for (size_t i = 0; i < n; ++i) if (!flags[i]) partitioned.push_back(values[i]);

    uint64_t hash = 14695981039346656037ULL;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 14695981039346656037ULL;
        const uint32_t* p = partitioned.data();
        for (size_t i = 0; i < n; ++i) {
            h = mix(h, p[i]);
        }
        hash = h;
    }
    return hash;
}