#include "interface.h"
#include <vector>
#include <cstring>

uint64_t groupby_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups, int iters) {
    std::vector<uint64_t> sums(groups, 0);
    uint64_t out = 0;
    const size_t n = keys.size();
    const uint32_t* kptr = keys.data();
    const uint32_t* vptr = values.data();
    uint64_t* sptr = sums.data();
    const size_t g = static_cast<size_t>(groups);

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(sptr, 0, g * sizeof(uint64_t));
        for (size_t i = 0; i < n; ++i) {
            sptr[kptr[i]] += vptr[i];
        }

        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < g; ++i) {
            hash ^= sptr[i];
            hash *= 1099511628211ULL;
        }
        out = hash;
    }
    return out;
}
