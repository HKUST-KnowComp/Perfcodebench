#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

// Compute base^exp using exponentiation by squaring (mod 2^64)
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

// Compute (p^n - 1) / (p - 1) mod 2^64
// This is 1 + p + p^2 + ... + p^(n-1)
// We use the identity: sum = (p^n - 1) * inverse(p - 1)
// Since p - 1 is even, we need a different approach.
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
        uint32_t half = n >> 1;
        uint64_t s_half = geometric_sum(p, half);
        uint64_t p_half = power(p, half);
        return (1 + p_half) * s_half;
    }
}

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    // Precompute segment lengths
    const size_t num_segments = offsets.size() > 0 ? offsets.size() - 1 : 0;
    
    if (num_segments == 0) {
        // No segments, just return the initial hash after iters iterations
        return FNV_OFFSET;
    }
    
    // Precompute lengths
    std::vector<uint32_t> lengths(num_segments);
    for (size_t s = 0; s < num_segments; ++s) {
        lengths[s] = offsets[s + 1] - offsets[s];
    }
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        
        // For each segment s with length len:
        // We need to compute: hash = ((hash ^ s) * p) repeated len times
        // Let h0 = hash, then after len iterations:
        // h_len = (h0 ^ s) * p^len + s * (p^len - 1) / (p - 1) * p - s * (p^len - 1) / (p - 1)
        // Actually, let's derive it properly:
        // h1 = (h0 ^ s) * p
        // h2 = (h1 ^ s) * p = ((h0 ^ s) * p ^ s) * p = (h0 ^ s) * p^2 ^ s * p
        // h3 = (h2 ^ s) * p = ((h0 ^ s) * p^2 ^ s * p ^ s) * p = (h0 ^ s) * p^3 ^ s * p^2 ^ s * p
        // ...
        // h_n = (h0 ^ s) * p^n ^ s * (p^(n-1) + p^(n-2) + ... + p)
        //     = (h0 ^ s) * p^n ^ s * p * (p^(n-1) - 1) / (p - 1)
        //     = (h0 ^ s) * p^n ^ s * (p^n - p) / (p - 1)
        // Using geometric series: p + p^2 + ... + p^(n-1) = p * (1 + p + ... + p^(n-2)) = p * S(n-1)
        // So: h_n = (h0 ^ s) * p^n ^ s * p * S(n-1) where S(k) = 1 + p + ... + p^(k-1)
        // Simplify: h_n = (h0 ^ s) * p^n ^ s * (S(n) - 1) since p * S(n-1) = S(n) - 1 when S(n) = 1 + p*S(n-1)
        // Wait, S(n) = 1 + p + p^2 + ... + p^(n-1), so p * S(n-1) = p + p^2 + ... + p^(n-1) = S(n) - 1
        // Therefore: h_n = (h0 ^ s) * p^n ^ (s * (S(n) - 1))
        //                = h0 * p^n ^ s * p^n ^ s * S(n) ^ s
        //                = h0 * p^n ^ s * (p^n ^ S(n) ^ 1)
        //                = h0 * p^n ^ s * (p^n + S(n) + 1) -- NO, XOR not addition
        // Let's just use: h_n = (h0 ^ s) * p^n ^ s * (S(n) - 1)
        
        for (size_t s = 0; s < num_segments; ++s) {
            uint32_t len = lengths[s];
            if (len == 0) continue;
            
            uint64_t seg_id = static_cast<uint64_t>(s);
            uint64_t p_n = power(FNV_PRIME, len);
            uint64_t s_n = geometric_sum(FNV_PRIME, len);  // 1 + p + p^2 + ... + p^(len-1)
            
            // h_n = (h0 ^ s) * p^n ^ s * (S(n) - 1)
            // where S(n) - 1 = p + p^2 + ... + p^(n-1) = p * S(n-1)
            // But S(n) - 1 = s_n - 1
            hash = (hash ^ seg_id) * p_n ^ (seg_id * (s_n - 1));
        }
    }
    
    return hash;
}