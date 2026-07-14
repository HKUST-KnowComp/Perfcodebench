#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

static inline void store_u32(uint8_t* dst, uint32_t v) {
  dst[0] = (uint8_t)v;
  dst[1] = (uint8_t)(v >> 8);
  dst[2] = (uint8_t)(v >> 16);
  dst[3] = (uint8_t)(v >> 24);
}

static inline void store_u16(uint8_t* dst, uint16_t v) {
  dst[0] = (uint8_t)v;
  dst[1] = (uint8_t)(v >> 8);
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint8_t* out = (uint8_t*)malloc(count * 12u);
  if (!out) return 0;
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out;
    for (size_t i = 0; i < count; ++i) {
      store_u32(ptr, ids[i]); ptr += 4;
      store_u32(ptr, ts[i]); ptr += 4;
      store_u16(ptr, levels[i]); ptr += 2;
      store_u16(ptr, lens[i]); ptr += 2;
    }
    hash = checksum_bytes(out, count * 12u);
  }
  free(out);
  return hash;
}
