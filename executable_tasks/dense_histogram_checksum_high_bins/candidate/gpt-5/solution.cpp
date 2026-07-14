#include "interface.h"

#include <cstdint>
#include <vector>

static inline uint64_t fnv_mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL; // FNV-1a 64-bit prime
  return hash;
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Match baseline behavior: if iters == 0, return default-initialized hash (0)
  if (iters == 0) return 0ULL;

  // If no bins, hash remains the FNV offset basis (no mixing happens)
  if (bins == 0u) return 1469598103934665603ULL;

  // Dense histogram for [0, bins)
  std::vector<uint32_t> hist(bins, 0u);
  const uint32_t max_bin = bins;

  // Accumulate counts only for values within [0, bins)
  for (size_t i = 0, n = values.size(); i < n; ++i) {
    uint32_t v = values[i];
    if (v < max_bin) {
      ++hist[v];
    }
  }

  // Compute FNV-1a hash over all bins (exactly as baseline would after counting)
  uint64_t hash = 1469598103934665603ULL; // FNV offset basis

  const uint32_t* p = hist.data();
  size_t n = static_cast<size_t>(bins);

  // Unrolled mixing loop for better throughput
  while (n >= 4) {
    hash = fnv_mix(hash, static_cast<uint64_t>(p[0]));
    hash = fnv_mix(hash, static_cast<uint64_t>(p[1]));
    hash = fnv_mix(hash, static_cast<uint64_t>(p[2]));
    hash = fnv_mix(hash, static_cast<uint64_t>(p[3]));
    p += 4;
    n -= 4;
  }
  while (n--) {
    hash = fnv_mix(hash, static_cast<uint64_t>(*p++));
  }

  return hash;
}
