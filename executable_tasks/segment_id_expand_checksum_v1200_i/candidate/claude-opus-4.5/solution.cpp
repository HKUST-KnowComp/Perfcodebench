#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

// Compute base^exp mod 2^64 using exponentiation by squaring
inline uint64_t power_mod(uint64_t base, uint64_t exp) {
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

// Compute (1 + p + p^2 + ... + p^(n-1)) mod 2^64
// This equals (p^n - 1) / (p - 1), but we need modular arithmetic
// We use the recurrence: S(n) = 1 + p * S(n-1)
// Or binary splitting: S(2k) = S(k) * (1 + p^k), S(2k+1) = 1 + p * S(2k)
inline uint64_t geometric_sum(uint64_t p, uint64_t n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    
    uint64_t half = n >> 1;
    uint64_t s_half = geometric_sum(p, half);
    uint64_t p_half = power_mod(p, half);
    
    if (n & 1) {
        // S(2k+1) = 1 + p * (S(k) * (1 + p^k))
        return 1 + p * s_half * (1 + p_half);
    } else {
        // S(2k) = S(k) * (1 + p^k)
        return s_half * (1 + p_half);
    }
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (offsets.size() < 2) {
        return FNV_OFFSET;
    }
    
    const size_t num_segments = offsets.size() - 1;
    
    // Precompute segment lengths
    std::vector<uint32_t> lengths(num_segments);
    for (size_t s = 0; s < num_segments; ++s) {
        lengths[s] = offsets[s + 1] - offsets[s];
    }
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        
        // For each segment s with length len:
        // We apply: hash = (hash ^ s) * p, len times
        // After len iterations:
        // hash_new = hash * p^len + s * (p + p^2 + ... + p^len)
        //          = hash * p^len + s * p * (1 + p + ... + p^(len-1))
        //          = hash * p^len + s * p * geometric_sum(p, len)
        
        for (size_t s = 0; s < num_segments; ++s) {
            uint32_t len = lengths[s];
            if (len == 0) continue;
            
            uint64_t seg_id = static_cast<uint64_t>(s);
            uint64_t p_len = power_mod(FNV_PRIME, len);
            uint64_t geo = geometric_sum(FNV_PRIME, len);
            
            hash = hash * p_len + seg_id * FNV_PRIME * geo;
        }
    }
    
    return hash;
}