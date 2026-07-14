#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

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

// Compute (1 + p + p^2 + ... + p^(n-1)) mod 2^64
// Using the identity: sum = (p^n - 1) / (p - 1)
// Since we're in mod 2^64, we need modular inverse of (p-1)
// For FNV_PRIME, (FNV_PRIME - 1) is even, so no direct inverse exists.
// Instead, use recursive formula:
// S(n) = 1 + p + p^2 + ... + p^(n-1)
// If n is even: S(n) = (1 + p^(n/2)) * S(n/2)
// If n is odd:  S(n) = 1 + p * S(n-1)
inline uint64_t geom_sum(uint64_t p, uint32_t n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    if (n & 1) {
        // S(n) = 1 + p * S(n-1)
        return 1 + p * geom_sum(p, n - 1);
    } else {
        // S(n) = (1 + p^(n/2)) * S(n/2)
        uint64_t half = geom_sum(p, n >> 1);
        uint64_t p_half = power(p, n >> 1);
        return (1 + p_half) * half;
    }
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    if (offsets.size() < 2) {
        // No segments, just return the initial hash repeated
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
        // 
        // Let h0 = hash before segment, s = segment id, p = FNV_PRIME
        // After 1 step:  h1 = (h0 ^ s) * p
        // After 2 steps: h2 = (h1 ^ s) * p = ((h0 ^ s) * p ^ s) * p
        //                   = (h0 ^ s) * p^2 ^ s * p
        // After n steps: hn = (h0 ^ s) * p^n ^ s * (p^(n-1) + p^(n-2) + ... + p)
        //                   = (h0 ^ s) * p^n ^ s * p * S(n-1)
        //                   = h0 * p^n ^ s * p^n ^ s * p * S(n-1)
        //                   = h0 * p^n ^ s * (p^n + p * S(n-1))
        //                   = h0 * p^n ^ s * (p^n + p + p^2 + ... + p^(n-1))
        //                   = h0 * p^n ^ s * (1 + p + p^2 + ... + p^n - 1)
        //                   = h0 * p^n ^ s * (S(n+1) - 1)
        // Actually let's re-derive more carefully:
        // h1 = (h0 ^ s) * p = h0*p ^ s*p
        // h2 = (h1 ^ s) * p = h1*p ^ s*p = h0*p^2 ^ s*p^2 ^ s*p
        // h3 = h2*p ^ s*p = h0*p^3 ^ s*p^3 ^ s*p^2 ^ s*p
        // hn = h0*p^n ^ s*(p^n + p^(n-1) + ... + p)
        //    = h0*p^n ^ s*p*(p^(n-1) + p^(n-2) + ... + 1)
        //    = h0*p^n ^ s*p*S(n)
        
        for (size_t s = 0; s < num_segments; ++s) {
            uint32_t len = lengths[s];
            if (len == 0) continue;
            
            uint64_t seg_id = static_cast<uint64_t>(s);
            uint64_t p_n = power(FNV_PRIME, len);
            uint64_t sum_n = geom_sum(FNV_PRIME, len);
            
            // hn = h0 * p^n ^ s * p * S(n)
            hash = hash * p_n ^ seg_id * FNV_PRIME * sum_n;
        }
    }
    
    return hash;
}