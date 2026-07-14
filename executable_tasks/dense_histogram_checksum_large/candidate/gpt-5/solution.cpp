#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int iters) {
  // Use dense arrays with generation-based lazy clearing to avoid per-iteration memset/fill
  std::vector<uint32_t> counts(bins);
  std::vector<uint32_t> stamps(bins, 0u);

  constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
  constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

  uint64_t hash = 0;
  uint32_t gen = 1u;

  const size_t n = values.size();

  for (int iter = 0; iter < iters; ++iter) {
    // Handle extremely rare wrap-around of generation counter
    if (gen == 0u) {
      // Reset stamps to ensure correctness after 2^32 iterations
      std::fill(stamps.begin(), stamps.end(), 0u);
      gen = 1u;
    }
    const uint32_t current_gen = gen++;

    // Accumulate counts for in-range values using generation stamping
    for (size_t i = 0; i < n; ++i) {
      uint32_t v = values[i];
      if (v < bins) {
        if (stamps[v] != current_gen) {
          stamps[v] = current_gen;
          counts[v] = 1u;
        } else {
          counts[v] += 1u;
        }
      }
    }

    // Compute FNV-1a-like checksum over all bins [0, bins)
    uint64_t h = FNV_OFFSET;
    for (uint32_t b = 0; b < bins; ++b) {
      uint64_t val = (stamps[b] == current_gen) ? static_cast<uint64_t>(counts[b]) : 0ull;
      h ^= val;
      h *= FNV_PRIME;
    }
    hash = h;
  }

  return hash;
}
