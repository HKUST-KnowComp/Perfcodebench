#include "interface.h"
#include <vector>
#include <algorithm>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> agg(groups);
    uint64_t out = 0;
    const std::size_t n = keys.size();
    const uint32_t* k_data = keys.data();
    const uint32_t* v_data = values.data();
    uint64_t* a_data = agg.data();

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(a_data, a_data + groups, 0);
        for (std::size_t i = 0; i < n; ++i) {
            a_data[k_data[i]] += v_data[i];
        }
        
        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            hash ^= a_data[g];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }
    return out;
}
