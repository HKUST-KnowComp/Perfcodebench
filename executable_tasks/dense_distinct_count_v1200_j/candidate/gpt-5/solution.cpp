#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  if (iters <= 0) return 0;
  if (key_space == 0) return 0;

  // Use a bitset: one bit per possible key in [0, key_space).
  const uint64_t words = (static_cast<uint64_t>(key_space) + 63u) >> 6;
  std::vector<uint64_t> bits(words, 0);

  uint64_t distinct = 0;
  for (uint32_t k : keys) {
    if (k >= key_space) continue; // Defensive: inputs should be within bound.
    const uint64_t idx = static_cast<uint64_t>(k) >> 6;
    const uint64_t mask = 1ULL << (k & 63u);
    const uint64_t w = bits[idx];
    if ((w & mask) == 0) {
      bits[idx] = w | mask;
      ++distinct;
    }
  }

  return distinct;
}
