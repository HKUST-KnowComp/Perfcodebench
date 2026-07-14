#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> sums(groups, 0);
    uint64_t out = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    const uint32_t* k_data = keys.data();
    const uint32_t* v_data = values.data();
    const size_t n = keys.size();
    uint64_t* s_data = sums.data();

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(s_data, s_data + groups, 0);
        for (size_t i = 0; i < n; ++i) {
            s_data[k_data[i]] += static_cast<uint64_t>(v_data[i]);
        }

        uint64_t hash = FNV_OFFSET;
        for (int g = 0; g < groups; ++g) {
            hash ^= s_data[g];
            hash *= FNV_PRIME;
        }
        out = hash;
    }
    return out;
}