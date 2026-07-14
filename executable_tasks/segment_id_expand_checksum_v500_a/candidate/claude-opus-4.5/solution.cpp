#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

// Compute FNV_PRIME^n mod 2^64 using binary exponentiation
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
    uint64_t total_len = 0;
    for (size_t s = 0; s < num_segments; ++s) {
        lengths[s] = offsets[s + 1] - offsets[s];
        total_len += lengths[s];
    }
    
    // Precompute cumulative lengths from the end (for power computation)
    // suffix_len[s] = sum of lengths[s+1..num_segments-1]
    std::vector<uint64_t> suffix_len(num_segments + 1);
    suffix_len[num_segments] = 0;
    for (size_t s = num_segments; s > 0; --s) {
        suffix_len[s - 1] = suffix_len[s] + lengths[s - 1];
    }
    
    // Precompute powers of FNV_PRIME for suffix lengths
    // power_suffix[s] = FNV_PRIME^suffix_len[s+1]
    std::vector<uint64_t> power_suffix(num_segments);
    for (size_t s = 0; s < num_segments; ++s) {
        power_suffix[s] = power_mod(FNV_PRIME, static_cast<uint32_t>(suffix_len[s + 1]));
    }
    
    // For each segment s with length L and segment_id s:
    // It contributes: s * (1 + p + p^2 + ... + p^(L-1)) * p^(suffix_len[s+1])
    // where p = FNV_PRIME
    // 
    // The hash formula is:
    // hash = FNV_OFFSET * p^total_len + sum over all segments of their contributions
    // But actually the FNV hash is computed as:
    // hash = ((hash ^ v) * p) for each value v
    // Starting with hash = FNV_OFFSET
    // 
    // Let's derive: after processing values v0, v1, ..., v(n-1):
    // hash = FNV_OFFSET * p^n + v0 * p^n + v1 * p^(n-1) + ... + v(n-1) * p
    // Wait, let me re-derive:
    // h0 = FNV_OFFSET
    // h1 = (h0 ^ v0) * p = h0 * p + v0 * p  (since XOR with 0s in h0 gives v0 for low bits)
    // Actually XOR is not multiplication, so we can't simplify like that.
    // 
    // Let me reconsider. The mix function is:
    // hash = (hash ^ value) * FNV_PRIME
    // 
    // This is NOT linear, so we can't use the geometric series trick directly.
    // We need to actually compute the hash, but we can avoid storing the expanded array.
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t s = 0; s < num_segments; ++s) {
            const uint32_t len = lengths[s];
            const uint32_t seg_id = static_cast<uint32_t>(s);
            for (uint32_t i = 0; i < len; ++i) {
                hash ^= seg_id;
                hash *= FNV_PRIME;
            }
        }
    }
    
    return hash;
}