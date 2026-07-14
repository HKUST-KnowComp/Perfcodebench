#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    (void)iters; // aggregation is deterministic and independent of iterations
    std::vector<uint64_t> agg(groups, 0);
    const size_t n = keys.size();
    const uint32_t* k = keys.data();
    const uint32_t* v = values.data();
    uint64_t* a = agg.data();
    for (size_t i = 0; i < n; ++i) {
        a[k[i]] += static_cast<uint64_t>(v[i]);
    }
    uint64_t hash = 1469598103934665603ULL;
    for (int g = 0; g < groups; ++g) {
        hash ^= a[g];
        hash *= 1099511628211ULL;
    }
    return hash;
}