#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint32_t value) {
    hash ^= static_cast<uint64_t>(value);
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters)
{
    const size_t n = values.size();
    std::vector<uint32_t> out(n);
    uint64_t hash = 14695981039346656037ULL;

    for (int iter = 0; iter < iters; ++iter) {
        size_t left = 0, right = n;
        for (size_t i = 0; i < n; ++i) {
            uint32_t v = values[i];
            bool pred = flags[i];
            size_t idx = pred ? left++ : --right;
            out[idx] = v;
        }
        for (size_t i = 0; i < n; ++i) {
            hash = mix(hash, out[i]);
        }
    }
    return hash;
}