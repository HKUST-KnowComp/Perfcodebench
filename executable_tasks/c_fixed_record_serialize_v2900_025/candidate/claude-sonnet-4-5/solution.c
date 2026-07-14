#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t* data64 = (const uint64_t*)data;
  size_t n64 = n / 8;
  size_t i;
  
  for (i = 0; i < n64; ++i) {
    uint64_t val = data64[i];
    hash ^= (uint8_t)val;
    hash *= 1099511628211ULL;
    hash ^= (uint8_t)(val >> 8);
    hash *= 1099511628211ULL;
    hash ^= (uint8_t)(val >> 16);
    hash *= 1099511628211ULL;
    hash ^= (uint8_t)(val >> 24);
    hash *= 1099511628211ULL;
    hash ^= (uint8_t)(val >> 32);
    hash *= 1099511628211ULL;
    hash ^= (uint8_t)(val >> 40);
    hash *= 1099511628211ULL;
    hash ^= (uint8_t)(val >> 48);
    hash *= 1099511628211ULL;
    hash ^= (uint8_t)(val >> 56);
    hash *= 1099511628211ULL;
  }
  
  for (i = n64 * 8; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t hash = 0;
  size_t size = count * 12;
  uint8_t* out = (uint8_t*)malloc(size);
  if (!out) exit(4);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* p = out;
    for (size_t i = 0; i < count; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t level = levels[i];
      uint16_t len = lens[i];
      
      p[0] = (uint8_t)id;
      p[1] = (uint8_t)(id >> 8);
      p[2] = (uint8_t)(id >> 16);
      p[3] = (uint8_t)(id >> 24);
      p[4] = (uint8_t)t;
      p[5] = (uint8_t)(t >> 8);
      p[6] = (uint8_t)(t >> 16);
      p[7] = (uint8_t)(t >> 24);
      p[8] = (uint8_t)level;
      p[9] = (uint8_t)(level >> 8);
      p[10] = (uint8_t)len;
      p[11] = (uint8_t)(len >> 8);
      p += 12;
    }
    hash = checksum_bytes(out, size);
  }
  free(out);
  return hash;
}