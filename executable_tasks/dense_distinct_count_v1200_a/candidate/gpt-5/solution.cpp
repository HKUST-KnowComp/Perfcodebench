#include "interface.h"

#include <unordered_set>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  (void)iters;
  // Handle empty inputs quickly
  if (keys.empty()) return 0ULL;

  // If key_space is zero, no keys are within bounds; fall back to exact set counting
  if (key_space == 0u) {
    std::unordered_set<uint32_t> us;
    us.reserve(keys.size());
    for (uint32_t k : keys) us.insert(k);
    return static_cast<uint64_t>(us.size());
  }

  // Use a dense bitset for fast bounded distinct counting.
  const uint64_t words = (static_cast<uint64_t>(key_space) + 63u) >> 6; // ceil(key_space/64)
  std::vector<uint64_t> bits;
  bits.assign(static_cast<size_t>(words), 0ULL);

  uint64_t count = 0;
  for (uint32_t k : keys) {
    if (k >= key_space) {
      // Out-of-range key detected; fall back to exact hash set counting for correctness.
      std::unordered_set<uint32_t> us;
      us.reserve(keys.size());
      for (uint32_t kk : keys) us.insert(kk);
      return static_cast<uint64_t>(us.size());
    }
    const uint32_t idx = k >> 6;            // word index
    const uint64_t mask = 1ULL << (k & 63); // bit mask within word
    uint64_t v = bits[idx];
    if ((v & mask) == 0ULL) {
      bits[idx] = v | mask;
      ++count;
    }
  }

  return count;
}
