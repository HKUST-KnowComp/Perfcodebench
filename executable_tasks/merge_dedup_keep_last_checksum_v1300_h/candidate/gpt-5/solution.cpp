#include "interface.h"

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
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
    int /*iters*/) {
  // Compute checksum once using a linear merge with keep-last semantics.
  const std::size_t nL = left_keys.size();
  const std::size_t nR = right_keys.size();

  std::size_t i = 0;
  std::size_t j = 0;

  uint64_t hash = 1469598103934665603ULL; // FNV offset basis

  while (i < nL || j < nR) {
    if (i < nL && (j >= nR || left_keys[i] < right_keys[j])) {
      uint32_t k = left_keys[i];
      uint32_t v = left_values[i];
      ++i;
      while (i < nL && left_keys[i] == k) { v = left_values[i]; ++i; }
      hash = mix(hash, static_cast<uint64_t>(k));
      hash = mix(hash, static_cast<uint64_t>(v));
    } else if (j < nR && (i >= nL || left_keys[i] > right_keys[j])) {
      uint32_t k = right_keys[j];
      uint32_t v = right_values[j];
      ++j;
      while (j < nR && right_keys[j] == k) { v = right_values[j]; ++j; }
      hash = mix(hash, static_cast<uint64_t>(k));
      hash = mix(hash, static_cast<uint64_t>(v));
    } else { // keys are equal and both i<nL and j<nR
      uint32_t k = left_keys[i]; // == right_keys[j]

      // Consume all duplicates for this key in left, keeping last value
      uint32_t vL = left_values[i];
      ++i;
      while (i < nL && left_keys[i] == k) { vL = left_values[i]; ++i; }

      // Consume all duplicates for this key in right, keeping last value
      uint32_t vR = right_values[j];
      ++j;
      while (j < nR && right_keys[j] == k) { vR = right_values[j]; ++j; }

      // Keep-last across runs: right overrides left for equal keys
      hash = mix(hash, static_cast<uint64_t>(k));
      hash = mix(hash, static_cast<uint64_t>(vR));
    }
  }

  return hash;
}
