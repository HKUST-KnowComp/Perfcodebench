#include "interface.h"
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t sliding_window_max_hash(const std::vector<uint32_t>& values, int window, int iters) {
  const size_t n = values.size();
  const size_t w = static_cast<size_t>(window);

  if (n < w || w == 0) {
    return 0;
  }

  const size_t num_windows = n - w + 1;
  std::vector<uint32_t> max_values(num_windows);

  if (w == 1) {
    // Optimization for window size 1
    for (size_t i = 0; i < num_windows; ++i) {
      max_values[i] = values[i];
    }
  } else {
    // Block-based sliding window maximum algorithm (O(N))
    // Divide array into blocks of size 'window'. 
    // Precompute prefix max from start of block and suffix max from end of block.
    std::vector<uint32_t> prefix_max(n);
    std::vector<uint32_t> suffix_max(n);

    for (size_t i = 0; i < n; ++i) {
      if (i % w == 0) {
        prefix_max[i] = values[i];
      } else {
        prefix_max[i] = std::max(prefix_max[i - 1], values[i]);
      }
    }

    for (size_t i = n; i > 0; --i) {
      size_t idx = i - 1;
      if (idx == n - 1 || (idx + 1) % w == 0) {
        suffix_max[idx] = values[idx];
      } else {
        suffix_max[idx] = std::max(suffix_max[idx + 1], values[idx]);
      }
    }

    for (size_t i = 0; i < num_windows; ++i) {
      max_values[i] = std::max(suffix_max[i], prefix_max[i + w - 1]);
    }
  }

  // Perform hashing iterations
  uint64_t final_hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    // The compiler can often unroll or optimize this loop
    for (size_t i = 0; i < num_windows; ++i) {
      h ^= static_cast<uint64_t>(max_values[i]);
      h *= 1099511628211ULL;
    }
    final_hash = h;
  }

  return final_hash;
}
