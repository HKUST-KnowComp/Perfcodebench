#include "interface.h"
#include <stdint.h>
#include <stdlib.h>

static inline uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t hash = 0;
  const size_t total_bytes = count * 12;
  uint8_t* out = NULL;
  
  if (total_bytes > 0) {
    out = (uint8_t*)malloc(total_bytes);
    if (!out) exit(4);
  }

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* wptr = out;
    for (size_t i = 0; i < count; ++i) {
      // Write 32-bit ID (LSB first, matches original byte order)
      uint32_t id = ids[i];
      *wptr++ = (uint8_t)id;
      *wptr++ = (uint8_t)(id >> 8);
      *wptr++ = (uint8_t)(id >> 16);
      *wptr++ = (uint8_t)(id >> 24);
      // Write 32-bit timestamp
      uint32_t t = ts[i];
      *wptr++ = (uint8_t)t;
      *wptr++ = (uint8_t)(t >> 8);
      *wptr++ = (uint8_t)(t >> 16);
      *wptr++ = (uint8_t)(t >> 24);
      // Write 16-bit level
      uint16_t lvl = levels[i];
      *wptr++ = (uint8_t)lvl;
      *wptr++ = (uint8_t)(lvl >> 8);
      // Write 16-bit length
      uint16_t len = lens[i];
      *wptr++ = (uint8_t)len;
      *wptr++ = (uint8_t)(len >> 8);
    }
    hash = checksum_bytes(out, total_bytes);
  }

  free(out);
  return hash;
}