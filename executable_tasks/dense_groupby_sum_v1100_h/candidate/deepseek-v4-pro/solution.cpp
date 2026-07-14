#include "interface.h"

#include <cstring>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> agg(groups, 0);
    uint64_t out = 0;
    const std::size_t n = keys.size();
    const uint32_t* kptr = keys.data();
    const uint32_t* vptr = values.data();
    const std::size_t gsize = static_cast<std::size_t>(groups);

    for (int iter = 0; iter < iters; ++iter) {
        // Zero the aggregation array using fast memset
        std::memset(agg.data(), 0, gsize * sizeof(uint64_t));

        // Accumulate values for keys within [0, groups)
        for (std::size_t i = 0; i < n; ++i) {
            const uint32_t key = kptr[i];
            if (key < gsize) {
                agg[key] += static_cast<uint64_t>(vptr[i]);
            }
        }

        // Compute checksum over all groups
        uint64_t hash = 1469598103934665603ULL;
        for (std::size_t g = 0; g < gsize; ++g) {
            hash ^= agg[g];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }

    return out;
}
