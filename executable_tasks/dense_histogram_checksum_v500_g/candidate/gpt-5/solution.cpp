#include "interface.h"

namespace {
static inline uint64_t mix64(uint64_t h, uint64_t v) {
  h ^= v;
  h *= 1099511628211ULL; // FNV-1a 64-bit prime
  return h;
}
}

uint64_t histogram_checksum(const std::vector<uint32_t>& values, uint32_t bins, int /*iters*/) {
  // Dense histogram for bins [0, bins)
  std::vector<uint32_t> counts(bins, 0u);
  for (uint32_t v : values) {
    if (v < bins) ++counts[v];
  }

  // Compute checksum using the same mixing as the baseline
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis
  for (uint32_t b = 0; b < bins; ++b) {
    hash = mix64(hash, static_cast<uint64_t>(counts[b]));
  }
  return hash;
}
