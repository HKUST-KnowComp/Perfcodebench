#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Compute base^exp mod 2^64
inline uint64_t power(uint64_t base, uint32_t exp) {
    uint64_t result = 1;
    while (exp > 0) {
        if (exp & 1) {
            result *= base;
        }
        base *= base;
        exp >>= 1;
    }
    return result;
}

// After applying mix(hash, s) len times starting from hash:
// hash_new = hash * FNV_PRIME^len + s * (FNV_PRIME^len - 1) / (FNV_PRIME - 1)
// But division in modular arithmetic requires modular inverse.
// 
// Let's derive: mix(h, s) = (h ^ s) * FNV_PRIME
// This is NOT h * FNV_PRIME + s * FNV_PRIME, so we can't use geometric series directly.
// 
// We need to compute iteratively but can unroll for speed.

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= FNV_PRIME;
    return hash;
}

// Apply mix(hash, s) len times
inline uint64_t apply_mix_n_times(uint64_t hash, uint32_t s, uint32_t len) {
    // Unroll loop for better performance
    uint64_t sv = static_cast<uint64_t>(s);
    while (len >= 8) {
        hash ^= sv; hash *= FNV_PRIME;
        hash ^= sv; hash *= FNV_PRIME;
        hash ^= sv; hash *= FNV_PRIME;
        hash ^= sv; hash *= FNV_PRIME;
        hash ^= sv; hash *= FNV_PRIME;
        hash ^= sv; hash *= FNV_PRIME;
        hash ^= sv; hash *= FNV_PRIME;
        hash ^= sv; hash *= FNV_PRIME;
        len -= 8;
    }
    while (len > 0) {
        hash ^= sv;
        hash *= FNV_PRIME;
        --len;
    }
    return hash;
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    const std::size_t num_segments = offsets.size() > 0 ? offsets.size() - 1 : 0;
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t s = 0; s < num_segments; ++s) {
            const uint32_t len = offsets[s + 1] - offsets[s];
            hash = apply_mix_n_times(hash, static_cast<uint32_t>(s), len);
        }
    }
    return hash;
}