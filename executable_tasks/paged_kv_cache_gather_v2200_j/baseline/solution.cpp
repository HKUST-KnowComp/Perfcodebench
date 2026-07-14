#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int num_pages,
    int page_size,
    int iters) {
  std::vector<std::vector<uint32_t>> pages(static_cast<std::size_t>(num_pages), std::vector<uint32_t>(static_cast<std::size_t>(page_size)));
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int p = 0; p < num_pages; ++p) {
      for (int s = 0; s < page_size; ++s) {
        pages[static_cast<std::size_t>(p)][static_cast<std::size_t>(s)] =
            cache[static_cast<std::size_t>(p) * static_cast<std::size_t>(page_size) + static_cast<std::size_t>(s)];
      }
    }
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < page_ids.size(); ++i) {
      hash = mix(hash, pages[static_cast<std::size_t>(page_ids[i])][static_cast<std::size_t>(slot_offsets[i])]);
    }
  }
  return hash;
}
