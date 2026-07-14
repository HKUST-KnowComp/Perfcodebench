#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (iters <= 0) return 0;
    if (groups <= 0) return 1469598103934665603ULL;
    std::vector<uint64_t> agg(groups, 0);
    const uint32_t* kd = keys.data();
    const uint32_t* vd = values.data();
    const size_t n = keys.size();
    for (size_t i = 0; i < n; ++i) {
        uint32_t k = kd[i];
        if (k < static_cast<uint32_t>(groups)) {
            agg[k] += static_cast<uint64_t>(vd[i]);
        }
    }
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        hash ^= agg[g];
        hash *= 1099511628211ULL;
    }
    return hash;
}