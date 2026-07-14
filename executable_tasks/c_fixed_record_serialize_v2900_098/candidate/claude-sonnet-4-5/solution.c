#include "interface.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t prime = 1099511628211ULL;
  
  // Unroll with 8 accumulators for ILP
  uint64_t h0 = hash, h1 = hash, h2 = hash, h3 = hash;
  uint64_t h4 = hash, h5 = hash, h6 = hash, h7 = hash;
  
  size_t i = 0;
  size_t limit = n - (n % 8);
  
  for (; i < limit; i += 8) {
    h0 ^= (uint64_t)data[i];
    h0 *= prime;
    h1 ^= (uint64_t)data[i+1];
    h1 *= prime;
    h2 ^= (uint64_t)data[i+2];
    h2 *= prime;
    h3 ^= (uint64_t)data[i+3];
    h3 *= prime;
    h4 ^= (uint64_t)data[i+4];
    h4 *= prime;
    h5 ^= (uint64_t)data[i+5];
    h5 *= prime;
    h6 ^= (uint64_t)data[i+6];
    h6 *= prime;
    h7 ^= (uint64_t)data[i+7];
    h7 *= prime;
  }
  
  // Combine accumulators
  hash = h0 ^ h1 ^ h2 ^ h3 ^ h4 ^ h5 ^ h6 ^ h7;
  
  // Handle remainder
  for (; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= prime;
  }
  
  return hash;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t hash = 0;
  
  // Pre-allocate exact size: 12 bytes per record
  size_t total_size = count * 12;
  uint8_t* out = (uint8_t*)malloc(total_size);
  if (!out) exit(4);
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out;
    
    for (size_t i = 0; i < count; ++i) {
      // Write 4-byte id (little-endian)
      uint32_t id = ids[i];
      memcpy(ptr, &id, 4);
      ptr += 4;
      
      // Write 4-byte timestamp (little-endian)
      uint32_t t = ts[i];
      memcpy(ptr, &t, 4);
      ptr += 4;
      
      // Write 2-byte level (little-endian)
      uint16_t level = levels[i];
      memcpy(ptr, &level, 2);
      ptr += 2;
      
      // Write 2-byte len (little-endian)
      uint16_t len = lens[i];
      memcpy(ptr, &len, 2);
      ptr += 2;
    }
    
    hash = checksum_bytes(out, total_size);
  }
  
  free(out);
  return hash;
}
