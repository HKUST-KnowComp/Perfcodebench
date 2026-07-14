#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  std::vector<uint32_t> counts(bins, 0);
  uint64_t hash = 0;
  const uint32_t* __restrict vdata = values.data();
  const size_t n = values.size();
  uint32_t* __restrict cdata = counts.data();

  for (int iter = 0; iter < iters; ++iter) {
    std::memset(cdata, 0, bins * sizeof(uint32_t));

    size_t i = 0;
    const size_t unroll = 8;
    const size_t limit = n - (n % unroll);
    for (; i < limit; i += unroll) {
      ++cdata[vdata[i]];
      ++cdata[vdata[i + 1]];
      ++cdata[vdata[i + 2]];
      ++cdata[vdata[i + 3]];
      ++cdata[vdata[i + 4]];
      ++cdata[vdata[i + 5]];
      ++cdata[vdata[i + 6]];
      ++cdata[vdata[i + 7]];
    }
    for (; i < n; ++i) {
      ++cdata[vdata[i]];
    }

    hash = 1469598103934665603ULL;
    uint32_t b = 0;
    const uint32_t blimit = bins - (bins % 4);
    for (; b < blimit; b += 4) {
      hash = mix(hash, cdata[b]);
      hash = mix(hash, cdata[b + 1]);
      hash = mix(hash, cdata[b + 2]);
      hash = mix(hash, cdata[b + 3]);
    }
    for (; b < bins; ++b) {
      hash = mix(hash, cdata[b]);
    }
  }
  return hash;
}