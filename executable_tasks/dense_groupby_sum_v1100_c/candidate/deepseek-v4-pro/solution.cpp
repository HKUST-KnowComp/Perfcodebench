#include "interface.h"

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    if (groups <= 0) {
        return 1469598103934665603ULL;
    }
    const uint64_t hash_init = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;
    std::vector<uint64_t> agg(groups, 0);
    const uint32_t* __restrict keys_ptr = keys.data();
    const uint32_t* __restrict vals_ptr = values.data();
    const std::size_t n = keys.size();
    uint64_t out = 0;

    for (int iter = 0; iter < iters; ++iter) {
        for (int g = 0; g < groups; ++g) {
            agg[g] = 0;
        }
        for (std::size_t i = 0; i < n; ++i) {
            agg[keys_ptr[i]] += static_cast<uint64_t>(vals_ptr[i]);
        }
        uint64_t hash = hash_init;
        for (int g = 0; g < groups; ++g) {
            hash ^= agg[g];
            hash *= multiplier;
        }
        out = hash;
    }
    return out;
}
