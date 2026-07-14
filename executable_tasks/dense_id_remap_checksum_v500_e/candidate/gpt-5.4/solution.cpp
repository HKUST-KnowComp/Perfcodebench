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
  if (probes.empty() || iters <= 0) {
    return 0;
  }

  uint32_t max_id = 0;
  if (!keys.empty()) {
    max_id = keys.back();
  }
  for (uint32_t p : probes) {
    if (p > max_id) max_id = p;
  }

  std::vector<uint32_t> remap(static_cast<std::size_t>(max_id) + 1);
  const std::size_t n = keys.size();
  for (std::size_t i = 0; i < n; ++i) {
    remap[keys[i]] = values[i];
  }

  const uint32_t* probe_ptr = probes.data();
  const std::size_t probe_count = probes.size();
  const uint32_t* remap_ptr = remap.data();

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < probe_count; ++i) {
      hash ^= remap_ptr[probe_ptr[i]];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
