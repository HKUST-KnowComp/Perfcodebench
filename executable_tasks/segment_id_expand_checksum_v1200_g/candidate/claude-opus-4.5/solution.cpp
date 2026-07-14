#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static constexpr uint64_t FNV_PRIME = 1099511628211ULL;
static constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

// Modular inverse of (FNV_PRIME - 1) mod 2^64
// (FNV_PRIME - 1) * INV_PRIME_M1 ≡ 1 (mod 2^64)
static constexpr uint64_t INV_PRIME_M1 = 0x56CE5D8DFFFFFFFDULL;

// Fast exponentiation: base^exp mod 2^64
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

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    const std::size_t n = offsets.size();
    if (n <= 1) {
        return FNV_OFFSET;
    }

    // Precompute segment lengths
    const std::size_t num_segments = n - 1;
    std::vector<uint32_t> lengths(num_segments);
    for (std::size_t s = 0; s < num_segments; ++s) {
        lengths[s] = offsets[s + 1] - offsets[s];
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        
        // For each segment s with length len, we need to apply:
        // hash = mix(hash, s) repeated len times
        // mix(h, s) = (h ^ s) * FNV_PRIME
        //
        // After len iterations starting from hash h with value s:
        // h_new = h * c^len + s * c * (c^len - 1) / (c - 1)
        // where c = FNV_PRIME
        //
        // Let g = (c^len - 1) / (c - 1) = 1 + c + c^2 + ... + c^(len-1)
        // Then: h_new = h * c^len + s * c * g
        //             = h * c^len + s * (c^len - 1) * INV_PRIME_M1 * c
        //             = h * c^len + s * (c^(len+1) - c) * INV_PRIME_M1
        
        for (std::size_t s = 0; s < num_segments; ++s) {
            const uint32_t len = lengths[s];
            if (len == 0) continue;
            
            const uint64_t seg_id = static_cast<uint64_t>(s);
            const uint64_t c_len = power(FNV_PRIME, len);
            
            // g = (c^len - 1) / (c - 1) mod 2^64
            // Using modular inverse: g = (c^len - 1) * INV_PRIME_M1
            const uint64_t g = (c_len - 1) * INV_PRIME_M1;
            
            // h_new = h * c^len + s * c * g
            hash = hash * c_len + seg_id * FNV_PRIME * g;
        }
    }
    return hash;
}