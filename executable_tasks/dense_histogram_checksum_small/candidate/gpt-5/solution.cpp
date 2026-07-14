#include "interface.h"

namespace {

static inline uint64_t mix64(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL; // FNV-1a 64-bit prime
  return h;
}

}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  uint64_t hash = 0; // Preserve behavior: if iters == 0, return 0
  std::vector<uint32_t> counts(bins);

  const uint32_t* vals = values.data();
  const size_t nvals = values.size();
  const size_t B = bins;

  for (int iter = 0; iter < iters; ++iter) {
    // Clear counts
    {
      size_t i = 0;
      size_t B8 = B & ~size_t(7);
      for (; i < B8; i += 8) {
        counts[i + 0] = 0;
        counts[i + 1] = 0;
        counts[i + 2] = 0;
        counts[i + 3] = 0;
        counts[i + 4] = 0;
        counts[i + 5] = 0;
        counts[i + 6] = 0;
        counts[i + 7] = 0;
      }
      for (; i < B; ++i) counts[i] = 0;
    }

    // Accumulate histogram (ignore values >= bins)
    {
      size_t i = 0;
      size_t n4 = nvals & ~size_t(3);
      for (; i < n4; i += 4) {
        uint32_t v0 = vals[i + 0]; if (v0 < bins) ++counts[v0];
        uint32_t v1 = vals[i + 1]; if (v1 < bins) ++counts[v1];
        uint32_t v2 = vals[i + 2]; if (v2 < bins) ++counts[v2];
        uint32_t v3 = vals[i + 3]; if (v3 < bins) ++counts[v3];
      }
      for (; i < nvals; ++i) {
        uint32_t v = vals[i];
        if (v < bins) ++counts[v];
      }
    }

    // Compute checksum via FNV-1a style mixing over bins in order [0, bins)
    uint64_t h = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
    {
      const uint32_t* c = counts.data();
      size_t i = 0;
      size_t B4 = B & ~size_t(3);
      for (; i < B4; i += 4) {
        h = mix64(h, c[i + 0]);
        h = mix64(h, c[i + 1]);
        h = mix64(h, c[i + 2]);
        h = mix64(h, c[i + 3]);
      }
      for (; i < B; ++i) {
        h = mix64(h, c[i]);
      }
    }

    hash = h;
  }

  return hash;
}
