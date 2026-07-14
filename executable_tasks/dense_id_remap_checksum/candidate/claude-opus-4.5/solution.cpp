#include "interface.h"

#include <cstdint>
#include <vector>
#include <algorithm>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (keys.empty() || probes.empty() || iters <= 0) {
    return 0;
  }

  // Build direct lookup table: table[key - min_key] = value
  const uint32_t min_key = keys.front();
  const uint32_t max_key = keys.back();
  const size_t table_size = static_cast<size_t>(max_key - min_key) + 1;

  std::vector<uint32_t> table(table_size);
  const size_t n = keys.size();
  for (size_t i = 0; i < n; ++i) {
    table[keys[i] - min_key] = values[i];
  }

  const uint32_t* __restrict tbl = table.data();
  const uint32_t* __restrict prb = probes.data();
  const size_t probe_count = probes.size();

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    size_t i = 0;
    const size_t unroll_limit = probe_count & ~static_cast<size_t>(3);

    // Unrolled loop (4 at a time)
    for (; i < unroll_limit; i += 4) {
      uint32_t v0 = tbl[prb[i]     - min_key];
      uint32_t v1 = tbl[prb[i + 1] - min_key];
      uint32_t v2 = tbl[prb[i + 2] - min_key];
      uint32_t v3 = tbl[prb[i + 3] - min_key];
      hash = mix(hash, v0);
      hash = mix(hash, v1);
      hash = mix(hash, v2);
      hash = mix(hash, v3);
    }

    // Remainder
    for (; i < probe_count; ++i) {
      hash = mix(hash, tbl[prb[i] - min_key]);
    }
  }

  return hash;
}
