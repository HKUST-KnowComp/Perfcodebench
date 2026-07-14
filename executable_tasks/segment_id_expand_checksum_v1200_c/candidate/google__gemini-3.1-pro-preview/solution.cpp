#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t segment_expand_hash(const std::vector<uint32_t>& offsets, int iters) {
  if (iters <= 0) {
    return 0;
  }
  
  uint64_t hash = 1469598103934665603ULL;
  if (offsets.size() < 2) {
    return hash;
  }

  const uint32_t* data = offsets.data();
  const std::size_t num_segments = offsets.size() - 1;
  
  for (std::size_t s = 0; s < num_segments; ++s) {
    const uint32_t len = data[s + 1] - data[s];
    const uint64_t value = static_cast<uint32_t>(s);
    for (uint32_t i = 0; i < len; ++i) {
      hash ^= value;
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}
