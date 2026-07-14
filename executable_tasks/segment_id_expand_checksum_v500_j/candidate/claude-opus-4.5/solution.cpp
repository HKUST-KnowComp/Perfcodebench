#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

constexpr uint64_t FNV_PRIME = 1099511628211ULL;
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;

}  // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) {
    return FNV_OFFSET;
  }
  
  const std::size_t num_segments = offsets.size() - 1;
  
  // Precompute segment lengths
  std::vector<uint32_t> lengths(num_segments);
  for (std::size_t s = 0; s < num_segments; ++s) {
    lengths[s] = offsets[s + 1] - offsets[s];
  }
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    
    // For each segment s with length len, we apply:
    //   hash = ((hash ^ s) * P ^ s) * P ... len times
    // Which expands to:
    //   hash = hash * P^len  ^  s * (P^(len-1) + P^(len-2) + ... + P + 1)
    //        = hash * P^len  ^  s * (P^len - 1) / (P - 1)
    
    for (std::size_t s = 0; s < num_segments; ++s) {
      uint32_t len = lengths[s];
      if (len == 0) continue;
      
      uint64_t seg_id = static_cast<uint64_t>(s);
      
      // Compute P^len using binary exponentiation
      uint64_t p_pow_len = 1;
      uint64_t base = FNV_PRIME;
      uint32_t exp = len;
      while (exp > 0) {
        if (exp & 1) {
          p_pow_len *= base;
        }
        base *= base;
        exp >>= 1;
      }
      
      // Compute geometric series sum: (P^len - 1) / (P - 1) mod 2^64
      // We need modular inverse of (P - 1) mod 2^64
      // P - 1 = 1099511628210 = 2 * 549755814105
      // Since P-1 is even, (P-1) has no inverse mod 2^64
      // 
      // Alternative: compute sum directly using divide and conquer
      // sum(n) = 1 + P + P^2 + ... + P^(n-1)
      // If n is even: sum(n) = (1 + P^(n/2)) * sum(n/2)
      // If n is odd:  sum(n) = 1 + P * sum(n-1)
      
      // Iterative version using binary representation
      // sum(n) where we build up using the recurrence
      uint64_t geo_sum = 0;
      uint64_t p_power = 1;  // P^0 = 1
      uint64_t current_sum = 0;  // sum so far
      uint64_t current_pow = 1;  // P^(bits processed so far)
      
      // We compute sum = 1 + P + P^2 + ... + P^(len-1)
      // Using: if we have sum(k) = 1+P+...+P^(k-1), then
      // sum(2k) = sum(k) + P^k * sum(k) = sum(k) * (1 + P^k)
      // sum(2k+1) = sum(2k) + P^(2k) = sum(k)*(1+P^k) + P^(2k)
      
      uint32_t n = len;
      // Find highest bit
      if (n > 0) {
        // Build from MSB to LSB
        int bits = 32 - __builtin_clz(n);
        uint64_t acc_sum = 0;  // accumulated sum
        uint64_t acc_pow = 1;  // P^(accumulated length)
        
        for (int b = bits - 1; b >= 0; --b) {
          // Double: acc_sum = acc_sum * (1 + acc_pow), acc_pow = acc_pow^2
          acc_sum = acc_sum * (1 + acc_pow);
          acc_pow = acc_pow * acc_pow;
          
          if ((n >> b) & 1) {
            // Add one: acc_sum = acc_sum + acc_pow, acc_pow = acc_pow * P
            acc_sum = acc_sum + acc_pow;
            acc_pow = acc_pow * FNV_PRIME;
          }
        }
        geo_sum = acc_sum;
      }
      
      // hash = hash * P^len ^ (s * geo_sum)
      hash = hash * p_pow_len ^ (seg_id * geo_sum);
    }
  }
  
  return hash;
}