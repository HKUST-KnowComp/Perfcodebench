#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

// Modular inverse of (FNV_PRIME - 1) mod 2^64
// (FNV_PRIME - 1) * INV_P_MINUS_1 ≡ 1 (mod 2^64)
// FNV_PRIME - 1 = 1099511628210
// We need inverse of 1099511628210 mod 2^64
constexpr uint64_t INV_P_MINUS_1 = 0x0da3b29A86B99879ULL;

inline uint64_t power(uint64_t base, uint32_t exp) {
    uint64_t result = 1;
    while (exp > 0) {
        if (exp & 1) result *= base;
        base *= base;
        exp >>= 1;
    }
    return result;
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
    
    // Precompute powers of FNV_PRIME for each segment length
    // and the geometric series sum: (P^L - 1) / (P - 1) = (P^L - 1) * INV_P_MINUS_1
    std::vector<uint64_t> pow_p(num_segments);
    std::vector<uint64_t> geo_sum(num_segments);
    
    for (size_t s = 0; s < num_segments; ++s) {
        uint64_t p_L = power(FNV_PRIME, lengths[s]);
        pow_p[s] = p_L;
        // geo_sum = 1 + P + P^2 + ... + P^(L-1) = (P^L - 1) / (P - 1)
        geo_sum[s] = (p_L - 1) * INV_P_MINUS_1;
    }
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        
        // For each segment s with length L:
        // We need to compute hash after mixing s into hash L times.
        // mix(h, v) = (h ^ v) * P
        // 
        // After L iterations with same value v=s:
        // h_new = ((h ^ s) * P ^ s) * P ... (L times)
        // 
        // Let's derive: h_1 = (h_0 ^ s) * P = h_0 * P + s * P (since XOR with same bits)
        // Wait, XOR is not addition. Let me reconsider.
        //
        // Actually for XOR: (a ^ b) != a + b in general.
        // But we can still find a pattern:
        // h_1 = (h_0 ^ s) * P
        // h_2 = (h_1 ^ s) * P = ((h_0 ^ s) * P ^ s) * P
        //
        // Let's define: after mixing s once: h' = (h ^ s) * P
        // After mixing s twice: h'' = ((h ^ s) * P ^ s) * P
        //
        // For the specific case where we XOR with the same value repeatedly:
        // h_k = (h_{k-1} ^ s) * P
        //
        // This doesn't simplify nicely due to XOR. Let's just compute directly
        // but avoid the vector allocation.
        
        for (size_t s = 0; s < num_segments; ++s) {
            uint32_t seg_id = static_cast<uint32_t>(s);
            uint32_t len = lengths[s];
            for (uint32_t i = 0; i < len; ++i) {
                hash ^= seg_id;
                hash *= FNV_PRIME;
            }
        }
    }
    
    return hash;
}