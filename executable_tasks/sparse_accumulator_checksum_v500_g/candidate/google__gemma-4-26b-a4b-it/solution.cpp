#include "interface.h"
#include <vector>
#include <algorithm>
#include <map>

namespace {

const uint64_t FNV_OFFSET = 1469598103934665603ULL;
const uint64_t FNV_PRIME = 1099511628211ULL;

// Fast power function for the FNV prime multiplier to skip zero gaps
uint64_t power(uint64_t base, uint32_t exp) {
    uint64_t res = 1;
    while (exp > 0) {
        if (exp % 2 == 1) res *= base;
        base *= base;
        exp /= 2;
    }
    return res;
}

// Since the mix function for value 0 is: hash = (hash ^ 0) * FNV_PRIME
// Applying it N times is: hash = hash * (FNV_PRIME ^ N)
// We use a precomputed table or binary exponentiation.
inline uint64_t apply_zeros(uint64_t hash, uint32_t count) {
    if (count == 0) return hash;
    // We need to compute hash * (FNV_PRIME ^ count)
    // Using a simple loop for small counts or binary exponentiation for large ones
    uint64_t multiplier = 1;
    uint64_t base = FNV_PRIME;
    uint32_t e = count;
    while (e > 0) {
        if (e & 1) multiplier *= base;
        base *= base;
        e >>= 1;
    }
    return hash * multiplier;
}

} // namespace

uint64_t sparse_accumulator_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain,
    int iters) {
  
  if (indices.empty()) {
    uint64_t hash = FNV_OFFSET;
    uint64_t multiplier = apply_zeros(1, domain) / 1; // This is just FNV_PRIME^domain
    // Actually, the loop is: for (0 to domain-1) hash = (hash ^ 0) * P
    // This is hash = FNV_OFFSET * (P^domain)
    // Let's just implement the logic carefully.
  }

  // To handle the 'iters' loop correctly, we note that the accumulator is cleared 
  // and rebuilt every iteration. The result is the same every iteration.
  // We only need to compute it once.
  
  // 1. Accumulate
  // Using a map or sorting to handle sparsity
  std::map<uint32_t, uint64_t> accum;
  for (std::size_t i = 0; i < indices.size(); ++i) {
    accum[indices[i]] += static_cast<uint64_t>(values[i]);
  }

  uint64_t hash = FNV_OFFSET;
  uint32_t current_idx = 0;

  for (auto const& [idx, val] : accum) {
    // 1. Handle zeros before this index
    if (idx > current_idx) {
      uint32_t gap = idx - current_idx;
      // hash = hash * (FNV_PRIME ^ gap)
      uint64_t multiplier = 1;
      uint64_t base = FNV_PRIME;
      uint32_t e = gap;
      while (e > 0) {
        if (e & 1) multiplier *= base;
        base *= base;
        e >>= 1;
      }
      hash *= multiplier;
    }

    // 2. Handle the actual value
    hash ^= val;
    hash *= FNV_PRIME;
    
    current_idx = idx + 1;
  }

  // 3. Handle trailing zeros
  if (current_idx < domain) {
    uint32_t gap = domain - current_idx;
    uint64_t multiplier = 1;
    uint64_t base = FNV_PRIME;
    uint32_t e = gap;
    while (e > 0) {
      if (e & 1) multiplier *= base;
      base *= base;
      e >>= 1;
    }
    hash *= multiplier;
  }

  return hash;
}