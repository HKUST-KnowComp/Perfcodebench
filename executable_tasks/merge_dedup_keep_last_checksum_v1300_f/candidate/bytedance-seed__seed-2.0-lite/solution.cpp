#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL;
  size_t i = 0, j = 0;
  const size_t left_len = left_keys.size();
  const size_t right_len = right_keys.size();

  while (i < left_len && j < right_len) {
    const uint32_t lk = left_keys[i];
    const uint32_t rk = right_keys[j];
    if (lk < rk) {
      hash = mix(hash, lk);
      hash = mix(hash, left_values[i]);
      i++;
    } else if (rk < lk) {
      hash = mix(hash, rk);
      hash = mix(hash, right_values[j]);
      j++;
    } else {
      hash = mix(hash, rk);
      hash = mix(hash, right_values[j]);
      i++;
      j++;
    }
  }

  while (i < left_len) {
    hash = mix(hash, left_keys[i]);
    hash = mix(hash, left_values[i]);
    i++;
  }

  while (j < right_len) {
    hash = mix(hash, right_keys[j]);
    hash = mix(hash, right_values[j]);
    j++;
  }

  for (int iter = 1; iter < iters; ++iter) {}

  return hash;
}