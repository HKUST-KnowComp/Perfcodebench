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

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
    // Precompute segment lengths
    const std::size_t num_segments = offsets.size() > 0 ? offsets.size() - 1 : 0;
    
    if (num_segments == 0) {
        // No segments, just return the initial hash after iters iterations
        return 1469598103934665603ULL;
    }
    
    // Precompute lengths and powers of FNV_PRIME for each segment
    std::vector<uint32_t> lengths(num_segments);
    std::vector<uint64_t> prime_powers(num_segments);
    
    for (std::size_t s = 0; s < num_segments; ++s) {
        lengths[s] = offsets[s + 1] - offsets[s];
        prime_powers[s] = power(FNV_PRIME, lengths[s]);
    }
    
    // Precompute s * FNV_PRIME for each segment (used in geometric series)
    // For mix applied len times with value s:
    // hash_new = hash * p^len + s * (p^len - 1) / (p - 1) * p
    // But since we're in mod 2^64 and p-1 is even, we need a different approach.
    // 
    // Actually, let's derive it properly:
    // mix(h, s) = (h ^ s) * p
    // This is NOT h * p + s * p, it's (h XOR s) * p
    // XOR doesn't distribute nicely, so we can't use geometric series.
    // 
    // Let's just compute directly but avoid memory allocation.
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        
        for (std::size_t s = 0; s < num_segments; ++s) {
            const uint32_t len = lengths[s];
            const uint64_t seg_id = static_cast<uint64_t>(s);
            
            // Apply mix len times with value seg_id
            for (uint32_t i = 0; i < len; ++i) {
                hash ^= seg_id;
                hash *= FNV_PRIME;
            }
        }
    }
    
    return hash;
}