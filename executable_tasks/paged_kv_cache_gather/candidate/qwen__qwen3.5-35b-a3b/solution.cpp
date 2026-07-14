#pragma once

#include <cstdint>
#include <vector>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < page_ids.size(); ++i) {
    uint32_t value = cache[static_cast<std::size_t>(page_ids[i]) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(slot_offsets[i])];
    hash ^= value;
    hash *= 1099511628211ULL;
  }
  return hash;
}