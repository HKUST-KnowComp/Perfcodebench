#include "interface.h"
#include <algorithm>
#include <vector>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> sums(static_cast<size_t>(groups), 0);
    const uint32_t* kptr = keys.data();
    const uint32_t* vptr = values.data();
    size_t n = keys.size();
    uint64_t out = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(sums.begin(), sums.end(), 0);
        for (size_t i = 0; i < n; ++i) {
            sums[kptr[i]] += vptr[i];
        }
        uint64_t h = 1469598103934665603ULL;
        for (int g = 0; g < groups; ++g) {
            h ^= sums[g];
            h *= 1099511628211ULL;
        }
        out = h;
    }
    return out;
}