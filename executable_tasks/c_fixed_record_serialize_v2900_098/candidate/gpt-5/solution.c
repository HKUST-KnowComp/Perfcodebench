#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t hash = 0;
  if (iters <= 0) {
    return 0; // Match baseline behavior when no iterations
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL; // FNV-1 64-bit offset basis
    const uint64_t prime = 1099511628211ULL; // FNV-1 64-bit prime

    const uint32_t* pid = ids;
    const uint32_t* pts = ts;
    const uint16_t* plevels = levels;
    const uint16_t* plens = lens;

    for (size_t i = 0; i < count; ++i) {
      uint32_t v32;
      uint16_t v16;

      // id (4 bytes, little-endian)
      v32 = *pid++;
      h ^= (uint8_t)(v32);
      h *= prime;
      h ^= (uint8_t)(v32 >> 8);
      h *= prime;
      h ^= (uint8_t)(v32 >> 16);
      h *= prime;
      h ^= (uint8_t)(v32 >> 24);
      h *= prime;

      // ts (4 bytes, little-endian)
      v32 = *pts++;
      h ^= (uint8_t)(v32);
      h *= prime;
      h ^= (uint8_t)(v32 >> 8);
      h *= prime;
      h ^= (uint8_t)(v32 >> 16);
      h *= prime;
      h ^= (uint8_t)(v32 >> 24);
      h *= prime;

      // level (2 bytes, little-endian)
      v16 = *plevels++;
      h ^= (uint8_t)(v16);
      h *= prime;
      h ^= (uint8_t)(v16 >> 8);
      h *= prime;

      // len (2 bytes, little-endian)
      v16 = *plens++;
      h ^= (uint8_t)(v16);
      h *= prime;
      h ^= (uint8_t)(v16 >> 8);
      h *= prime;
    }

    hash = h; // Match baseline: hash of the last iteration
  }

  return hash;
}
