#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t partition_scatter_checksum(const std::vector<uint32_t>& keys, uint32_t groups, int iters) {
  std::vector<std::vector<uint32_t>> buckets(groups);
  std::vector<uint32_t> offsets(static_cast<std::size_t>(groups) + 1);
  std::vector<uint32_t> rows;
  
  // Pre-allocate reasonable capacity
  const std::size_t n = keys.size();
  rows.reserve(n);
  
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Clear buckets
    for (auto& bucket : buckets) {
      bucket.clear();
    }
    
    // Partition keys into buckets using direct indexing
    for (std::size_t i = 0; i < n; ++i) {
      buckets[keys[i]].push_back(static_cast<uint32_t>(i));
    }
    
    // Build offsets and flatten rows
    offsets[0] = 0;
    rows.clear();
    for (uint32_t g = 0; g < groups; ++g) {
      const uint32_t bucket_size = static_cast<uint32_t>(buckets[g].size());
      offsets[static_cast<std::size_t>(g + 1)] = offsets[g] + bucket_size;
      rows.insert(rows.end(), buckets[g].begin(), buckets[g].end());
    }
    
    // Compute checksum
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