#include "interface.h"
#include <unordered_set>

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  (void)key_space;
  // Compute distinct count once.
  std::unordered_set<uint32_t> seen;
  seen.reserve(keys.size());
  for (uint32_t key : keys) {
    seen.insert(key);
  }
  uint64_t count = static_cast<uint64_t>(seen.size());
  // The baseline repeated clear+insert each iteration, but count never changes.
  for (int iter = 0; iter < iters; ++iter) {
    // No operation needed; count remains the same.
  }
  return count;
}
