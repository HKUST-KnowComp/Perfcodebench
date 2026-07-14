#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  size_t i = 0;
  size_t unrolled = n & ~(size_t)7;
  
  for (; i < unrolled; i += 8) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
    hash ^= (uint64_t)data[i + 1];
    hash *= prime;
    hash ^= (uint64_t)data[i + 2];
    hash *= prime;
    hash ^= (uint64_t)data[i + 3];
    hash *= prime;
    hash ^= (uint64_t)data[i + 4];
    hash *= prime;
    hash ^= (uint64_t)data[i + 5];
    hash *= prime;
    hash ^= (uint64_t)data[i + 6];
    hash *= prime;
    hash ^= (uint64_t)data[i + 7];
    hash *= prime;
  }
  
  for (; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
  }
  
  return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t hash = 0;
  
  size_t total_size = count * 12;
  if (total_size == 0) {
    for (int iter = 0; iter < iters; ++iter) {
      hash = 1469598103934665603ULL;
    }
    return hash;
  }
  
  uint8_t* out = (uint8_t*)malloc(total_size);
  if (!out) exit(4);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out;
    
    for (size_t i = 0; i < count; ++i) {
      memcpy(ptr, &ids[i], 4);
      ptr += 4;
      memcpy(ptr, &ts[i], 4);
      ptr += 4;
      memcpy(ptr, &levels[i], 2);
      ptr += 2;
      memcpy(ptr, &lens[i], 2);
      ptr += 2;
    }
    
    hash = checksum_bytes(out, total_size);
  }
  
  free(out);
  return hash;
}