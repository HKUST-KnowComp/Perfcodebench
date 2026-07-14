#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  uint64_t hash = 0;
  if (iters <= 0) return hash;

  std::vector<uint32_t> counts(bins, 0);
  std::vector<uint32_t> touched;
  touched.reserve(values.size() < static_cast<size_t>(bins) ? values.size() : static_cast<size_t>(bins));

  for (int iter = 0; iter < iters; ++iter) {
    touched.clear();

    for (uint32_t v : values) {
      uint32_t& c = counts[v];
      if (c == 0) touched.push_back(v);
      ++c;
    }

    uint64_t h = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      h = mix(h, counts[b]);
    }
    hash = h;

    for (uint32_t v : touched) {
      counts[v] = 0;
    }
  }

  return hash;
}
