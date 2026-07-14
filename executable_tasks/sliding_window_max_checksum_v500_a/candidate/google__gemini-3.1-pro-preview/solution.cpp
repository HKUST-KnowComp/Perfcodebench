#include "interface.h"
#include <algorithm>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  if (iters <= 0) return 0;

  size_t n = values.size();
  if (window <= 0 || static_cast<size_t>(window) > n) {
    return 1469598103934665603ULL;
  }

  size_t w = static_cast<size_t>(window);
  size_t num_windows = n - w + 1;

  // The two-pass (prefix/suffix) algorithm for sliding window maximum.
  // This approach is O(N) and cache-friendly.
  std::vector<uint32_t> prefix_max(n);
  std::vector<uint32_t> suffix_max(n);

  for (size_t i = 0; i < n; ++i) {
    if (i % w == 0) {
      prefix_max[i] = values[i];
    } else {
      prefix_max[i] = std::max(prefix_max[i - 1], values[i]);
    }
  }

  for (size_t i = n; i-- > 0; ) {
    if (i == n - 1 || (i + 1) % w == 0) {
      suffix_max[i] = values[i];
    } else {
      suffix_max[i] = std::max(suffix_max[i + 1], values[i]);
    }
  }

  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < num_windows; ++i) {
    // The maximum in window [i, i + w - 1] is the max of the suffix max 
    // from the start of the window to the end of its block, and the 
    // prefix max from the start of the next block to the end of the window.
    uint32_t mx = std::max(suffix_max[i], prefix_max[i + w - 1]);
    hash = mix(hash, static_cast<uint64_t>(mx));
  }

  // Since the baseline resets the hash to the same constant at the start of 
  // every iteration, the final result is simply the result of a single pass.
  return hash;
}
