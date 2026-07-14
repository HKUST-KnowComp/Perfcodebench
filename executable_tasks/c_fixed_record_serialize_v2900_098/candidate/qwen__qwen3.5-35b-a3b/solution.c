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

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t total_size = 12 * count;
    uint8_t* out = (uint8_t*)malloc(total_size);
    if (!out) exit(4);
    uint8_t* ptr = out;
    for (size_t i = 0; i < count; ++i) {
      *ptr++ = (uint8_t)ids[i];
      *ptr++ = (uint8_t)(ids[i] >> 8);
      *ptr++ = (uint8_t)(ids[i] >> 16);
      *ptr++ = (uint8_t)(ids[i] >> 24);
      *ptr++ = (uint8_t)ts[i];
      *ptr++ = (uint8_t)(ts[i] >> 8);
      *ptr++ = (uint8_t)(ts[i] >> 16);
      *ptr++ = (uint8_t)(ts[i] >> 24);
      *ptr++ = (uint8_t)levels[i];
      *ptr++ = (uint8_t)(levels[i] >> 8);
      *ptr++ = (uint8_t)lens[i];
      *ptr++ = (uint8_t)(lens[i] >> 8);
    }
    hash = checksum_bytes(out, total_size);
    free(out);
  }
  return hash;
}