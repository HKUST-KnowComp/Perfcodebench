#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace {
static inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}
}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
    std::vector<uint32_t> max_vals(group_count);
    uint64_t hash = 0;
    const size_t n = groups.size();
    const uint32_t* grp = groups.data();
    const uint32_t* val = values.data();
    uint32_t* max_ptr = max_vals.data();

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(max_ptr, max_ptr + group_count, uint32_t(0));
        for (size_t i = 0; i < n; ++i) {
            uint32_t g = grp[i];
            uint32_t v = val[i];
            if (v > max_ptr[g]) {
                max_ptr[g] = v;
            }
        }
        hash = 1469598103934665603ULL;
        for (uint32_t g = 0; g < group_count; ++g) {
            hash = mix(hash, max_ptr[g]);
        }
    }
    return hash;
}
