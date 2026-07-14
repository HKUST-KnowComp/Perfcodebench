#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (offsets.size() < 2) {
    // If there are no segments to expand, the loop logic implies 
    // the hash is reset to the FNV offset basis each iteration.
    // However, the original code only updates 'hash' inside the loop 
    // if 'expanded' is populated. If 'expanded' is empty, 'hash' 
    // remains 0 from the initialization, but the loop logic 
    // suggests the last iteration's state is returned.
    // Following the baseline logic: if offsets.size() < 2, expanded is always empty.
    // The loop runs 'iters' times, and in each iteration, hash is set to 1469598103934665603ULL.
    // Since the inner loop for 'expanded' doesn't run, hash remains that constant.
    // But wait, the baseline code: hash = 1469598103934665603ULL is INSIDE the iter loop.
    // If expanded is empty, hash becomes 1469598103934665603ULL.
    // If iters is 0, it returns 0.
    if (iters <= 0) return 0;
    return 1469598103934665603ULL;
  }

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
      const uint32_t len = offsets[s + 1] - offsets[s];
      const uint64_t val = static_cast<uint64_t>(s);
      // Unrolling or using a simple loop to process the 'expanded' values
      for (uint32_t i = 0; i < len; ++i) {
        hash ^= val;
        hash *= 1099511628211ULL;
      }
    }
  }
  return hash;
}
