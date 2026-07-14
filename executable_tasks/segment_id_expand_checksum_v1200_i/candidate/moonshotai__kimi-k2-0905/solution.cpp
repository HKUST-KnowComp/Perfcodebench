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

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (offsets.size() <= 1 || iters <= 0) return 1469598103934665603ULL;

    // pre-compute the expanded segment-id stream once
    std::vector<uint32_t> expanded;
    expanded.reserve(offsets.back());          // exact final size
    for (size_t s = 0; s + 1 < offsets.size(); ++s) {
        const uint32_t len = offsets[s + 1] - offsets[s];
        expanded.insert(expanded.end(), len, static_cast<uint32_t>(s));
    }

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        // re-hash the identical buffer each iteration
        uint64_t h = 1469598103934665603ULL;
        const uint32_t* ptr = expanded.data();
        const uint32_t* end = ptr + expanded.size();
        while (ptr != end) {
            h = mix(h, *ptr++);
        }
        hash = h;   // last iteration wins, same as baseline
    }
    return hash;
}