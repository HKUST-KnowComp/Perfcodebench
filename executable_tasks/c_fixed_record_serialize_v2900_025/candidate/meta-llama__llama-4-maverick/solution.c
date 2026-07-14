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
  size_t record_size = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint16_t);
  size_t total_size = count * record_size;
  uint8_t* out = (uint8_t*)malloc(total_size);
  if (!out) exit(4);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out;
    for (size_t i = 0; i < count; ++i) {
      memcpy(ptr, &ids[i], sizeof(uint32_t));
      ptr += sizeof(uint32_t);
      memcpy(ptr, &ts[i], sizeof(uint32_t));
      ptr += sizeof(uint32_t);
      memcpy(ptr, &levels[i], sizeof(uint16_t));
      ptr += sizeof(uint16_t);
      memcpy(ptr, &lens[i], sizeof(uint16_t));
      ptr += sizeof(uint16_t);
    }
    hash = checksum_bytes(out, total_size);
  }
  free(out);
  return hash;
}