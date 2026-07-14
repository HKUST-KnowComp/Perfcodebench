#include "interface.h"

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters == 0) return 0;
  if (offsets.size() < 2) return 1469598103934665603ULL;
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t s = 0; s + 1 < offsets.size(); ++s) {
    uint32_t len = offsets[s + 1] - offsets[s];
    for (uint32_t i = 0; i < len; ++i) {
      hash ^= static_cast<uint64_t>(s);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
