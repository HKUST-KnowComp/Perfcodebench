#include "interface.h"
#include <vector>
#include <cstdint>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters)
{
    std::vector<uint32_t> best(group_count, 0);
    const size_t n = groups.size();
    uint64_t hash = 14695981039346656037ULL;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(best.begin(), best.end(), 0);

        size_t i = 0;
        // unroll 4x to reduce loop overhead and expose ILP
        for (; i + 3 < n; i += 4) {
            uint32_t g0 = groups[i];
            uint32_t g1 = groups[i+1];
            uint32_t g2 = groups[i+2];
            uint32_t g3 = groups[i+3];

            uint32_t v0 = values[i];
            uint32_t v1 = values[i+1];
            uint32_t v2 = values[i+2];
            uint32_t v3 = values[i+3];

            if (v0 > best[g0]) best[g0] = v0;
            if (v1 > best[g1]) best[g1] = v1;
            if (v2 > best[g2]) best[g2] = v2;
            if (v3 > best[g3]) best[g3] = v3;
        }
        for (; i < n; ++i) {
            uint32_t g = groups[i];
            uint32_t v = values[i];
            if (v > best[g]) best[g] = v;
        }

        // recompute hash from scratch each iteration
        uint64_t h = 14695981039346656037ULL;
        for (uint32_t g = 0; g < group_count; ++g) {
            h = mix(h, best[g]);
        }
        hash = h;
    }
    return hash;
}