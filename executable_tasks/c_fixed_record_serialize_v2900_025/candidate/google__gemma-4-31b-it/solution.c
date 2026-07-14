#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  if (count == 0) return 0;

  // Each record is exactly 12 bytes: 4 (id) + 4 (ts) + 2 (level) + 2 (len)
  size_t record_size = 12;
  size_t total_size = count * record_size;
  uint8_t* out = (uint8_t*)malloc(total_size);
  if (!out) return 0;

  uint8_t* ptr = out;
  for (size_t i = 0; i < count; ++i) {
    uint32_t id = ids[i];
    uint32_t t = ts[i];
    uint16_t lvl = levels[i];
    uint16_t ln = lens[i];

    *ptr++ = (uint8_t)id;
    *ptr++ = (uint8_t)(id >> 8);
    *ptr++ = (uint8_t)(id >> 16);
    *ptr++ = (uint8_t)(id >> 24);
    
    *ptr++ = (uint8_t)t;
    *ptr++ = (uint8_t)(t >> 8);
    *ptr++ = (uint8_t)(t >> 16);
    *ptr++ = (uint8_t)(t >> 24);
    
    *ptr++ = (uint8_t)lvl;
    *ptr++ = (uint8_t)(lvl >> 8);
    
    *ptr++ = (uint8_t)ln;
    *ptr++ = (uint8_t)(ln >> 8);
  }

  uint64_t hash = 0;
  // The data in 'out' is constant across iterations
  for (int iter = 0; iter < iters; ++iter) {
    hash = checksum_bytes(out, total_size);
  }

  free(out);
  return hash;
}