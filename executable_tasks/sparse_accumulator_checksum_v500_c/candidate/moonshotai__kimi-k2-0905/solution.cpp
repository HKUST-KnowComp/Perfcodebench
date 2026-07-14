#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters)
{
    // Reusable dense accumulator and touched-index list
    std::vector<uint64_t> accum(domain, 0);
    std::vector<uint32_t> touched;
    touched.reserve(indices.size());

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Clear only the positions we modified last iteration
        for (uint32_t idx : touched) accum[idx] = 0;
        touched.clear();

        // Apply all updates
        const size_t n = indices.size();
        for (size_t i = 0; i < n; ++i) {
            const uint32_t idx = indices[i];
            const uint64_t val = static_cast<uint64_t>(values[i]);
            if (accum[idx] == 0) touched.push_back(idx);
            accum[idx] += val;
        }

        // Compute checksum over the entire domain
        hash = 1469598103934665603ULL;
        for (uint32_t key = 0; key < domain; ++key) {
            hash = mix(hash, accum[key]);
        }
    }
    return hash;
}