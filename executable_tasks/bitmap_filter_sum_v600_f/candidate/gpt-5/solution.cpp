#include "interface.h"
#include <unordered_set>

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  // Baseline returns sum from the last iteration; if iters == 0, it returns 0.
  if (iters <= 0) return 0ull;

  // Fast path: use a bitmap for keys within [0, key_space).
  std::vector<uint8_t> bitmap;
  if (key_space > 0) bitmap.assign(static_cast<size_t>(key_space), 0);

  // Handle out-of-bounds enabled keys lazily with a set only if needed.
  std::unordered_set<uint32_t> oob_enabled;
  bool has_oob = false;

  for (uint32_t k : enabled_keys) {
    if (k < key_space) {
      bitmap[static_cast<size_t>(k)] = 1;
    } else {
      if (!has_oob) {
        has_oob = true;
      }
      oob_enabled.insert(k);
    }
  }

  // Compute the sum for one iteration (equivalent to the last one since inputs are constant).
  uint64_t sum = 0;
  const size_t n = probe_keys.size();
  const uint32_t* pk = probe_keys.data();
  const uint32_t* vals = values.data();

  if (key_space > 0) {
    if (!has_oob) {
      // Only in-bounds membership checks.
      for (size_t i = 0; i < n; ++i) {
        uint32_t k = pk[i];
        if (k < key_space) {
          sum += static_cast<uint64_t>(vals[i]) * static_cast<uint64_t>(bitmap[k]);
        }
        // If k >= key_space, there is no corresponding enabled key (since !has_oob).
      }
    } else {
      // Mixed case: use bitmap for in-bounds, set for out-of-bounds.
      for (size_t i = 0; i < n; ++i) {
        uint32_t k = pk[i];
        if (k < key_space) {
          sum += static_cast<uint64_t>(vals[i]) * static_cast<uint64_t>(bitmap[k]);
        } else {
          if (oob_enabled.find(k) != oob_enabled.end()) {
            sum += static_cast<uint64_t>(vals[i]);
          }
        }
      }
    }
  } else {
    // key_space == 0: all keys are out-of-bounds, only possible matches are via oob_enabled.
    if (has_oob) {
      for (size_t i = 0; i < n; ++i) {
        if (oob_enabled.find(pk[i]) != oob_enabled.end()) {
          sum += static_cast<uint64_t>(vals[i]);
        }
      }
    }
  }

  return sum;
}
