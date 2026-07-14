#include "interface.h"

#include <unordered_map>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  std::unordered_map<uint32_t, std::vector<uint32_t>> buckets;
  std::vector<uint32_t> offsets;
  std::vector<uint32_t> rows;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    buckets.clear();
    for (std::size_t i = 0; i < keys.size(); ++i) {
      buckets[keys[i]].push_back(static_cast<uint32_t>(i));
    }
    offsets.assign(static_cast<std::size_t>(groups) + 1, 0);
    rows.clear();
    for (uint32_t g = 0; g < groups; ++g) {
      const auto it = buckets.find(g);
      const uint32_t bucket_size =
          it == buckets.end() ? 0u : static_cast<uint32_t>(it->second.size());
      offsets[static_cast<std::size_t>(g + 1)] =
          offsets[static_cast<std::size_t>(g)] + bucket_size;
      if (it != buckets.end()) {
        rows.insert(rows.end(), it->second.begin(), it->second.end());
      }
    }
    hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
      hash = mix(hash, v);
    }
    for (uint32_t v : rows) {
      hash = mix(hash, v);
    }
  }
  return hash;
}
