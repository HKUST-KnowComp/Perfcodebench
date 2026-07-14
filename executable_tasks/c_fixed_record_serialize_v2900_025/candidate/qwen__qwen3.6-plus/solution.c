#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t hash = 0;
  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (size_t i = 0; i < count; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t l = levels[i];
      uint16_t ln = lens[i];

      hash ^= (uint64_t)(id & 0xFF);       hash *= FNV_PRIME;
      hash ^= (uint64_t)((id >> 8) & 0xFF); hash *= FNV_PRIME;
      hash ^= (uint64_t)((id >> 16) & 0xFF);hash *= FNV_PRIME;
      hash ^= (uint64_t)((id >> 24) & 0xFF);hash *= FNV_PRIME;

      hash ^= (uint64_t)(t & 0xFF);        hash *= FNV_PRIME;
      hash ^= (uint64_t)((t >> 8) & 0xFF); hash *= FNV_PRIME;
      hash ^= (uint64_t)((t >> 16) & 0xFF);hash *= FNV_PRIME;
      hash ^= (uint64_t)((t >> 24) & 0xFF);hash *= FNV_PRIME;

      hash ^= (uint64_t)(l & 0xFF);        hash *= FNV_PRIME;
      hash ^= (uint64_t)((l >> 8) & 0xFF); hash *= FNV_PRIME;

      hash ^= (uint64_t)(ln & 0xFF);       hash *= FNV_PRIME;
      hash ^= (uint64_t)((ln >> 8) & 0xFF);hash *= FNV_PRIME;
    }
  }
  return hash;
}
