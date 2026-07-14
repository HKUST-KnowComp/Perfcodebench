#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters)
{
    std::vector<uint64_t> accum(domain, 0);
    const size_t n = indices.size();
    const uint32_t* idx = indices.data();
    const uint32_t* val = values.data();

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        // bulk zero
        std::memset(accum.data(), 0, domain * sizeof(uint64_t));

        // accumulate updates
        for (size_t i = 0; i < n; ++i) {
            accum[idx[i]] += static_cast<uint64_t>(val[i]);
        }

        // compute hash
        hash = 1469598103934665603ULL;
        const uint64_t* p = accum.data();
        const uint64_t* end = p + domain;
        while (p != end) {
            hash = mix(hash, *p++);
        }
    }
    return hash;
}