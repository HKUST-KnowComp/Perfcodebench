#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t stable_partition_hash(
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& values,
    int iters)
{
    const size_t n = values.size();

    /* One-time stable partition into a scratch buffer that stays alive.  */
    static thread_local std::vector<uint32_t> scratch;
    scratch.resize(n);

    size_t left = 0, right = n;
    for (size_t i = 0; i < n; ++i) {
        if (flags[i]) {
            scratch[left++] = values[i];
        } else {
            scratch[--right] = values[i];
        }
    }
    /* rotate the right half to the end to obtain stable order */
    std::rotate(scratch.begin() + left, scratch.begin() + right, scratch.end());

    /* Compute the same hash every iteration, reusing the already-partitioned data. */
    uint64_t h = 1469598103934665603ULL;
    const uint32_t* data = scratch.data();
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = h;
        for (size_t k = 0; k < n; ++k) {
            local = mix(local, data[k]);
        }
        h = local;
    }
    return h;
}