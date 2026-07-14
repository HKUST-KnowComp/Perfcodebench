#include "interface.h"

#include <algorithm>
#include <vector>

uint64_t remap_checksum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = 1469598103934665603ULL;
  auto keys_begin = keys.begin();
  auto keys_end = keys.end();
  const uint32_t* values_data = values.data();

  for (uint32_t probe : probes) {
    auto it = std::lower_bound(keys_begin, keys_end, probe);
    uint64_t val = values_data[it - keys_begin];
    hash ^= val;
    hash *= 1099511628211ULL;
  }
  return hash;
}