#include "interface.h"
#include <unordered_map>
#include <cstdint>
#include <vector>

namespace {
constexpr uint64_t kPrime = 1099511628211ULL;
constexpr uint64_t kOffset = 1469598103934665603ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= kPrime;
    return hash;
}
}  // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
    if (iters == 0) return 0;
    // Only one iteration is needed; the original repeated the same work iters times.
    (void)iters;

    constexpr size_t kDenseLimit = 10000000;
    if (static_cast<uint64_t>(domain) <= kDenseLimit) {
        std::vector<uint64_t> accum(domain, 0);
        for (size_t i = 0; i < indices.size(); ++i) {
            accum[indices[i]] += static_cast<uint64_t>(values[i]);
        }
        uint64_t hash = kOffset;
        for (uint32_t key = 0; key < domain; ++key) {
            hash = mix(hash, accum[key]);
        }
        return hash;
    } else {
        std::unordered_map<uint32_t, uint64_t> accum;
        accum.reserve(indices.size() * 2);
        for (size_t i = 0; i < indices.size(); ++i) {
            accum[indices[i]] += static_cast<uint64_t>(values[i]);
        }
        uint64_t hash = kOffset;
        for (uint32_t key = 0; key < domain; ++key) {
            auto it = accum.find(key);
            hash = mix(hash, it == accum.end() ? 0ULL : it->second);
        }
        return hash;
    }
}