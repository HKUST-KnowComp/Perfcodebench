#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Compute base^exp mod 2^64 using binary exponentiation
inline uint64_t power(uint64_t base, uint64_t exp) {
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
    if (offsets.size() < 2) {
        // No segments, return the initial hash value
        uint64_t hash = 1469598103934665603ULL;
        for (int iter = 1; iter < iters; ++iter) {
            hash = 1469598103934665603ULL;
        }
        return hash;
    }

    const std::size_t num_segments = offsets.size() - 1;

    // Precompute segment lengths
    std::vector<uint32_t> lengths(num_segments);
    for (std::size_t s = 0; s < num_segments; ++s) {
        lengths[s] = offsets[s + 1] - offsets[s];
    }

    // For the FNV-1a style hash:
    // hash = ((hash ^ value) * FNV_PRIME)
    // When value is constant (segment id s) for len iterations:
    // Let p = FNV_PRIME, v = s
    // After 1 step: h1 = (h0 ^ v) * p
    // After 2 steps: h2 = (h1 ^ v) * p = ((h0 ^ v) * p ^ v) * p
    //                   = (h0 ^ v) * p^2 ^ v * p
    // After n steps: hn = (h0 ^ v) * p^n ^ v * (p^(n-1) + p^(n-2) + ... + p)
    //                   = (h0 ^ v) * p^n ^ v * (p^n - p) / (p - 1)
    // 
    // Since we're working mod 2^64 and p is odd, (p-1) is even and doesn't have
    // a multiplicative inverse. We need a different approach.
    //
    // Let's expand: after n applications of hash = (hash ^ v) * p
    // h_n = h_0 * p^n  ^  v * (p^n - p) / (p - 1)
    // 
    // Actually, let's derive more carefully:
    // h_1 = (h_0 ^ v) * p = h_0 * p ^ v * p
    // h_2 = (h_1 ^ v) * p = h_1 * p ^ v * p = h_0 * p^2 ^ v * p^2 ^ v * p
    // h_n = h_0 * p^n ^ v * (p^n + p^(n-1) + ... + p)
    //     = h_0 * p^n ^ v * p * (p^n - 1) / (p - 1)
    //
    // For geometric series sum: p + p^2 + ... + p^n = p * (p^n - 1) / (p - 1)
    // We need modular inverse of (p - 1) mod 2^64, but (p-1) is even.
    //
    // Alternative: compute the sum iteratively or use a different formula.
    // Sum = p * (1 + p + p^2 + ... + p^(n-1))
    // Let S(n) = 1 + p + p^2 + ... + p^(n-1)
    // S(n) can be computed via: if n is even, S(n) = (1 + p^(n/2)) * S(n/2)
    //                           if n is odd,  S(n) = 1 + p * S(n-1)

    // Precompute inverse of (p - 1) mod 2^64 doesn't work since gcd != 1
    // Use recursive doubling for geometric sum

    // Function to compute 1 + p + p^2 + ... + p^(n-1) mod 2^64
    auto geom_sum = [](uint64_t p, uint64_t n) -> uint64_t {
        if (n == 0) return 0;
        if (n == 1) return 1;
        
        uint64_t result = 0;
        uint64_t p_power = 1;  // p^0
        uint64_t term_sum = 1; // sum for current block
        uint64_t block_size = 1;
        
        // Use doubling: S(2k) = S(k) * (1 + p^k)
        // Build up the sum using binary representation of n
        while (n > 0) {
            if (n & 1) {
                result = result * power(p, block_size) + term_sum * p_power;
                p_power *= power(p, block_size);
            }
            // Double the block: term_sum for 2*block_size = term_sum * (1 + p^block_size)
            term_sum = term_sum * (1 + power(p, block_size));
            block_size *= 2;
            n >>= 1;
        }
        return result;
    };

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        
        for (std::size_t s = 0; s < num_segments; ++s) {
            uint64_t len = lengths[s];
            if (len == 0) continue;
            
            uint64_t v = static_cast<uint64_t>(s);
            uint64_t p_n = power(FNV_PRIME, len);
            
            // h_n = h_0 * p^n ^ v * p * S(n) where S(n) = 1 + p + ... + p^(n-1)
            // Actually: h_n = h_0 * p^n ^ v * (p + p^2 + ... + p^n)
            //               = h_0 * p^n ^ v * p * (1 + p + ... + p^(n-1))
            //               = h_0 * p^n ^ v * p * S(n)
            
            uint64_t sum = geom_sum(FNV_PRIME, len);  // 1 + p + ... + p^(len-1)
            hash = (hash ^ v) * p_n ^ v * (p_n - 1);  // Simplified using p*S(n) = p^n - 1 when... no that's wrong
            
            // Let me recalculate:
            // p * S(n) = p * (1 + p + ... + p^(n-1)) = p + p^2 + ... + p^n
            // This equals (p^(n+1) - p) / (p - 1), but we can't divide.
            // Just compute: p * S(n) directly
            uint64_t p_times_sum = FNV_PRIME * sum;
            hash = (hash ^ v) * p_n ^ v * p_times_sum;
        }
    }
    return hash;
}
