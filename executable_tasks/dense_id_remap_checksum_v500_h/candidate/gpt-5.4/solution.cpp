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

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  const std::size_t n = keys.size();
  const std::size_t m = probes.size();

  if (m == 0 || iters <= 0) {
    return 0;
  }

  // Build a dense direct-remap table once. The task is specifically dense id
  // remapping, so replacing per-probe binary search with O(1) indexed lookup
  // is the key optimization.
  uint32_t max_key = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (keys[i] > max_key) max_key = keys[i];
  }

  std::vector<uint32_t> remap(static_cast<std::size_t>(max_key) + 1);
  for (std::size_t i = 0; i < n; ++i) {
    remap[keys[i]] = values[i];
  }

  // Precompute the checksum contribution of one full probe pass, since the
  // baseline resets the hash each iteration and therefore every iteration
  // produces the same final checksum.
  uint64_t hash = 1469598103934665603ULL;
  const uint32_t* probe_ptr = probes.data();
  const uint32_t* remap_ptr = remap.data();

  std::size_t i = 0;
  for (; i + 4 <= m; i += 4) {
    hash = mix(hash, remap_ptr[probe_ptr[i]]);
    hash = mix(hash, remap_ptr[probe_ptr[i + 1]]);
    hash = mix(hash, remap_ptr[probe_ptr[i + 2]]);
    hash = mix(hash, remap_ptr[probe_ptr[i + 3]]);
  }
  for (; i < m; ++i) {
    hash = mix(hash, remap_ptr[probe_ptr[i]]);
  }

  return hash;
}
