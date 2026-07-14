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

uint64_t paged_kv_checksum(
    const std::vector<uint32_t>& cache,
    const std::vector<uint32_t>& page_ids,
    const std::vector<uint32_t>& slot_offsets,
    int /*num_pages*/,
    int page_size,
    int iters) {
  const std::size_t n = page_ids.size();
  if (n == 0) {
    return 1469598103934665603ULL;
  }

  const uint32_t* const cache_ptr = cache.data();
  const uint32_t* const page_ptr = page_ids.data();
  const uint32_t* const slot_ptr = slot_offsets.data();
  const std::size_t ps = static_cast<std::size_t>(page_size);

  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;

    std::size_t i = 0;

    if (ps == 1) {
      for (; i < n; ++i) {
        hash = mix(hash, cache_ptr[static_cast<std::size_t>(page_ptr[i])]);
      }
    } else if (ps == 2) {
      for (; i < n; ++i) {
        hash = mix(hash, cache_ptr[static_cast<std::size_t>(page_ptr[i]) * 2u + static_cast<std::size_t>(slot_ptr[i])]);
      }
    } else if (ps == 4) {
      for (; i < n; ++i) {
        hash = mix(hash, cache_ptr[static_cast<std::size_t>(page_ptr[i]) * 4u + static_cast<std::size_t>(slot_ptr[i])]);
      }
    } else if (ps == 8) {
      for (; i < n; ++i) {
        hash = mix(hash, cache_ptr[static_cast<std::size_t>(page_ptr[i]) * 8u + static_cast<std::size_t>(slot_ptr[i])]);
      }
    } else if (ps == 16) {
      for (; i < n; ++i) {
        hash = mix(hash, cache_ptr[static_cast<std::size_t>(page_ptr[i]) * 16u + static_cast<std::size_t>(slot_ptr[i])]);
      }
    } else if (ps == 32) {
      for (; i < n; ++i) {
        hash = mix(hash, cache_ptr[static_cast<std::size_t>(page_ptr[i]) * 32u + static_cast<std::size_t>(slot_ptr[i])]);
      }
    } else if (ps == 64) {
      for (; i < n; ++i) {
        hash = mix(hash, cache_ptr[static_cast<std::size_t>(page_ptr[i]) * 64u + static_cast<std::size_t>(slot_ptr[i])]);
      }
    } else if (ps == 128) {
      for (; i < n; ++i) {
        hash = mix(hash, cache_ptr[static_cast<std::size_t>(page_ptr[i]) * 128u + static_cast<std::size_t>(slot_ptr[i])]);
      }
    } else {
      for (; i < n; ++i) {
        hash = mix(hash, cache_ptr[static_cast<std::size_t>(page_ptr[i]) * ps + static_cast<std::size_t>(slot_ptr[i])]);
      }
    }
  }

  return hash;
}
