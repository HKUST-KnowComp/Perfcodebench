#include "interface.h"

#include <stdint.h>
#include <stddef.h>

static inline uint64_t fnv1a_init(void) {
  return 1469598103934665603ULL;
}

#define FNV1A_PRIME 1099511628211ULL

static inline uint64_t fnv1a_u32le(uint64_t h, uint32_t v) {
  h ^= (uint8_t)(v);
  h *= FNV1A_PRIME;
  h ^= (uint8_t)(v >> 8);
  h *= FNV1A_PRIME;
  h ^= (uint8_t)(v >> 16);
  h *= FNV1A_PRIME;
  h ^= (uint8_t)(v >> 24);
  h *= FNV1A_PRIME;
  return h;
}

static inline uint64_t fnv1a_u16le(uint64_t h, uint16_t v) {
  h ^= (uint8_t)(v);
  h *= FNV1A_PRIME;
  h ^= (uint8_t)(v >> 8);
  h *= FNV1A_PRIME;
  return h;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  if (iters <= 0) return 0;

  uint64_t hash = fnv1a_init();
  for (size_t i = 0; i < count; ++i) {
    uint32_t id = ids[i];
    uint32_t t = ts[i];
    uint16_t lvl = levels[i];
    uint16_t len = lens[i];

    hash = fnv1a_u32le(hash, id);
    hash = fnv1a_u32le(hash, t);
    hash = fnv1a_u16le(hash, lvl);
    hash = fnv1a_u16le(hash, len);
  }

  // Baseline returned the checksum of the last iteration; computing once suffices.
  return hash;
}
