#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

// Compute base^exp mod 2^64 using exponentiation by squaring
inline uint64_t power_mod(uint64_t base, uint32_t exp) {
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

// Compute (p^n - 1) / (p - 1) mod 2^64
// This is 1 + p + p^2 + ... + p^(n-1)
// We use the identity: sum = (p^n - 1) * inverse(p - 1)
// Since p-1 is even, we need a different approach.
// We compute the geometric series directly using divide and conquer:
// S(n) = 1 + p + p^2 + ... + p^(n-1)
// If n is even: S(n) = (1 + p^(n/2)) * S(n/2)
// If n is odd:  S(n) = 1 + p * S(n-1)
inline uint64_t geometric_sum(uint64_t p, uint32_t n) {
    if (n == 0) return 0;
    if (n == 1) return 1;
    if (n & 1) {
        // S(n) = 1 + p * S(n-1)
        return 1 + p * geometric_sum(p, n - 1);
    } else {
        // S(n) = (1 + p^(n/2)) * S(n/2)
        uint64_t half = geometric_sum(p, n >> 1);
        uint64_t p_half = power_mod(p, n >> 1);
        return half * (1 + p_half);
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
        
        // For each segment s with length len, we need to apply:
        // hash = ((hash ^ s) * p) repeated len times
        // 
        // After len iterations with the same value s:
        // hash_new = hash * p^len + s * (p^len - 1) / (p - 1) * p
        // But actually the recurrence is: h' = (h ^ s) * p
        // So: h1 = (h0 ^ s) * p = h0*p ^ s*p
        //     h2 = (h1 ^ s) * p = h1*p ^ s*p = h0*p^2 ^ s*p^2 ^ s*p
        //     ...
        //     hn = h0 * p^n ^ s * (p + p^2 + ... + p^n)
        //        = h0 * p^n ^ s * p * (1 + p + ... + p^(n-1))
        //        = h0 * p^n ^ s * p * S(n)
        // where S(n) = (p^n - 1) / (p - 1)
        
        for (size_t s = 0; s < num_segments; ++s) {
            uint32_t len = lengths[s];
            if (len == 0) continue;
            
            uint64_t seg_id = static_cast<uint64_t>(s);
            
            if (len <= 8) {
                // For small lengths, direct computation is faster
                for (uint32_t i = 0; i < len; ++i) {
                    hash ^= seg_id;
                    hash *= FNV_PRIME;
                }
            } else {
                // Use closed-form formula
                uint64_t p_n = power_mod(FNV_PRIME, len);
                uint64_t sum_n = geometric_sum(FNV_PRIME, len);
                hash = hash * p_n ^ (seg_id * FNV_PRIME * sum_n);
            }
        }
    }
    
    return hash;
}