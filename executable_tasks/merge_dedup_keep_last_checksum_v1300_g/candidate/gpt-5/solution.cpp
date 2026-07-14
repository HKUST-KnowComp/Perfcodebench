#include "interface.h"

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
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
    return 0ULL;
  }

  const std::size_t nl = left_keys.size();
  const std::size_t nr = right_keys.size();

  std::size_t i = 0;
  std::size_t j = 0;

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis

  while (i < nl || j < nr) {
    if (i < nl && (j >= nr || left_keys[i] < right_keys[j])) {
      // Consume a block of equal keys from left only
      const uint32_t key = left_keys[i];
      uint32_t v = left_values[i];
      ++i;
      while (i < nl && left_keys[i] == key) {
        v = left_values[i];
        ++i;
      }
      hash = mix(hash, static_cast<uint64_t>(key));
      hash = mix(hash, static_cast<uint64_t>(v));
    } else if (j < nr && (i >= nl || right_keys[j] < left_keys[i])) {
      // Consume a block of equal keys from right only
      const uint32_t key = right_keys[j];
      uint32_t v = right_values[j];
      ++j;
      while (j < nr && right_keys[j] == key) {
        v = right_values[j];
        ++j;
      }
      hash = mix(hash, static_cast<uint64_t>(key));
      hash = mix(hash, static_cast<uint64_t>(v));
    } else {
      // Keys are equal on both sides; keep-last => take last right value
      const uint32_t key = left_keys[i];  // == right_keys[j]
      // Advance left over all duplicates of key
      while (i < nl && left_keys[i] == key) {
        ++i;
      }
      // Advance right over all duplicates, capturing the last value
      uint32_t v = right_values[j];
      ++j;
      while (j < nr && right_keys[j] == key) {
        v = right_values[j];
        ++j;
      }
      hash = mix(hash, static_cast<uint64_t>(key));
      hash = mix(hash, static_cast<uint64_t>(v));
    }
  }

  return hash;
}
