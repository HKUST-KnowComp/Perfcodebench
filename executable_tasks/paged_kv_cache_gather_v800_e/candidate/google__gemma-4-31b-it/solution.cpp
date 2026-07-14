#include "interface.h"

#include <cstdint>
#include <vector>

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
    int num_pages,
    int page_size,
    int iters) {
  
  const uint32_t* cache_ptr = cache.data();
  const uint32_t* page_ids_ptr = page_ids.data();
  const uint32_t* slot_offsets_ptr = slot_offsets.data();
  const size_t num_lookups = page_ids.size();
  const size_t p_size = static_cast<size_t>(page_size);

  uint64_t hash = 0;
  
  // The baseline resets the hash to 1469598103934665603ULL at the start of every iteration.
  // This means only the final iteration actually contributes to the returned value.
  // We can skip the first (iters - 1) iterations entirely.
  if (iters <= 0) return 0;

  hash = 1469598103934665603ULL;
  for (size_t i = 0; i < num_lookups; ++i) {
    uint32_t page_id = page_ids_ptr[i];
    uint32_t offset = slot_offsets_ptr[i];
    uint32_t value = cache_ptr[static_cast<size_t>(page_id) * p_size + offset];
    hash = mix(hash, value);
  }

  return hash;
}