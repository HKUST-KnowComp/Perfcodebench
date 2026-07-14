#include "interface.h"
#include <vector>
#include <cstring>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> sums(groups, 0);
    uint64_t* sums_ptr = sums.data();
    const uint32_t* keys_ptr = keys.data();
    const uint32_t* vals_ptr = values.data();
    size_t n = keys.size();
    uint64_t out = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(sums_ptr, 0, static_cast<size_t>(groups) * sizeof(uint64_t));
        for (size_t i = 0; i < n; ++i) {
            sums_ptr[keys_ptr[i]] += vals_ptr[i];
        }
        uint64_t hash = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            hash ^= sums_ptr[g];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }
    return out;
}