#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static inline uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  size_t total_size = count * 12;
  uint8_t* out = (uint8_t*)malloc(total_size);
  if (!out && total_size > 0) exit(4);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out;
    for (size_t i = 0; i < count; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t lvl = levels[i];
      uint16_t ln = lens[i];
      
      ptr[0] = (uint8_t)id;
      ptr[1] = (uint8_t)(id >> 8);
      ptr[2] = (uint8_t)(id >> 16);
      ptr[3] = (uint8_t)(id >> 24);
      ptr[4] = (uint8_t)t;
      ptr[5] = (uint8_t)(t >> 8);
      ptr[6] = (uint8_t)(t >> 16);
      ptr[7] = (uint8_t)(t >> 24);
      ptr[8] = (uint8_t)lvl;
      ptr[9] = (uint8_t)(lvl >> 8);
      ptr[10] = (uint8_t)ln;
      ptr[11] = (uint8_t)(ln >> 8);
      ptr += 12;
    }
    hash = checksum_bytes(out, total_size);
  }
  free(out);
  return hash;
}