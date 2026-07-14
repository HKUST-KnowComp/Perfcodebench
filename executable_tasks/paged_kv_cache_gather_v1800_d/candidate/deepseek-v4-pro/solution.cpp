#include "interface.h"
#include <cstddef>
#include <vector>

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  const std::size_t num_gathers = page_ids.size();
  std::vector<std::size_t> offsets(num_gathers);
  const std::size_t page_size_t = static_cast<std::size_t>(page_size);
  for (std::size_t i = 0; i < num_gathers; ++i) {
    offsets[i] =
        static_cast<std::size_t>(page_ids[i]) * page_size_t +
        static_cast<std::size_t>(slot_offsets[i]);
  }

  uint64_t hash = 0;
  const uint32_t* cache_data = cache.data();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < num_gathers; ++i) {
      hash ^= cache_data[offsets[i]];
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}