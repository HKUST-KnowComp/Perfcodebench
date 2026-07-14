#include "interface.h"

#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct KV {
  uint32_t k;
  uint32_t v;
};

} // namespace

uint64_t merge_keep_last_hash(
    const std::vector<uint32_t>& left_keys, 
    const std::vector<uint32_t>& left_values, 
    const std::vector<uint32_t>& right_keys, 
    const std::vector<uint32_t>& right_values, 
    int iters) {
  
  const size_t n_left = left_keys.size();
  const size_t n_right = right_keys.size();
  
  // Pre-allocate buffer to avoid reallocations inside the loop
  // The maximum possible size of the merged unique set is n_left + n_right
  std::vector<KV> out;
  out.reserve(n_left + n_right);

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    size_t i = 0;
    size_t j = 0;

    while (i < n_left || j < n_right) {
      uint32_t current_key;
      uint32_t current_val;

      if (i < n_left && j < n_right) {
        if (left_keys[i] < right_keys[j]) {
          current_key = left_keys[i];
          current_val = left_values[i];
          i++;
        } else if (left_keys[i] > right_keys[j]) {
          current_key = right_keys[j];
          current_val = right_values[j];
          j++;
        } else {
          // Duplicate key: keep the right-side value
          current_key = right_keys[j];
          current_val = right_values[j];
          i++;
          j++;
        }
      } else if (i < n_left) {
        current_key = left_keys[i];
        current_val = left_values[i];
        i++;
      } else {
        current_key = right_keys[j];
        current_val = right_values[j];
        j++;
      }

      if (!out.empty() && out.back().k == current_key) {
        // This case handles duplicates within a single run if they exist,
        // though the problem implies runs are sorted and likely unique.
        // Given 'keep-last' semantics, we update the value.
        out.back().v = current_val;
      } else {
        out.push_back({current_key, current_val});
      }
    }

    hash = 1469598103934665603ULL;
    for (const auto& kv : out) {
      hash = mix(hash, static_cast<uint64_t>(kv.k));
      hash = mix(hash, static_cast<uint64_t>(kv.v));
    }
  }

  return hash;
}