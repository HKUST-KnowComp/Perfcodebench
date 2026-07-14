#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  (void)iters; // Work is independent of iters; compute once for performance
  const size_t n = keys.size();
  if (n == 0) return 0;

  // Heuristic: use a bitset when the key space is reasonably small compared to the input size
  // and when it fits within a conservative memory budget.
  const size_t BITSET_MAX_BYTES = 32ULL << 20; // 32 MB
  const size_t BITSET_MAX_BITS = BITSET_MAX_BYTES * 8ULL;

  if (key_space != 0 && static_cast<size_t>(key_space) <= BITSET_MAX_BITS && static_cast<size_t>(key_space) <= n * 64ULL) {
    const size_t word_count = (static_cast<size_t>(key_space) + 63) >> 6; // divide by 64, round up
    std::vector<uint64_t> bits(word_count, 0ULL);
    uint64_t count = 0;
    for (uint32_t k : keys) {
      const size_t idx = static_cast<size_t>(k) >> 6;
      const uint64_t mask = 1ULL << (k & 63U);
      uint64_t v = bits[idx];
      if ((v & mask) == 0) {
        bits[idx] = v | mask;
        ++count;
      }
    }
    return count;
  }

  // Fallback: sort-and-count unique
  std::vector<uint32_t> sorted(keys.begin(), keys.end());
  std::sort(sorted.begin(), sorted.end());
  uint64_t count = 1; // n >= 1
  for (size_t i = 1; i < n; ++i) {
    if (sorted[i] != sorted[i - 1]) ++count;
  }
  return count;
}
