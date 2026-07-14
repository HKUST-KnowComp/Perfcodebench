#include "interface.h"

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct BinCount {
  uint32_t count;
  uint32_t version;
};

} // namespace

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<BinCount> counts(bins);
  uint32_t version = 0;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    ++version;

    for (uint32_t v : values) {
      if (v < bins) {
        BinCount& bc = counts[v];
        if (bc.version == version) {
          ++bc.count;
        } else {
          bc.count = 1;
          bc.version = version;
        }
      }
    }

    hash = 1469598103934665603ULL;
    for (uint32_t b = 0; b < bins; ++b) {
      const BinCount& bc = counts[b];
      const uint32_t cnt = (bc.version == version) ? bc.count : 0;
      hash = mix(hash, cnt);
    }
  }

  return hash;
}
