#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int /*num_pages*/,
    int page_size,
    int iters) {

  const uint32_t* cache_data = cache.data();
  const uint32_t* id_data = page_ids.data();
  const uint32_t* off_data = slot_offsets.data();
  const std::size_t gather_count = page_ids.size();

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < gather_count; ++i) {
      uint32_t page = id_data[i];
      uint32_t slot = off_data[i];
      uint32_t value = cache_data[page * static_cast<std::size_t>(page_size) + slot];
      hash = mix(hash, value);
    }
  }
  return hash;
}