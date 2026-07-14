#include "interface.h"
#include <stdint.h>

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t final_hash = 0;
  const uint64_t hash_init = 1469598103934665603ULL;
  const uint64_t hash_mul  = 1099511628211ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = hash_init;
    for (size_t i = 0; i < count; ++i) {
      uint32_t id = ids[i];
      uint32_t t  = ts[i];
      uint16_t level = levels[i];
      uint16_t len   = lens[i];
      // id, LSB first
      h ^= (uint8_t)id;         h *= hash_mul;
      h ^= (uint8_t)(id >> 8);  h *= hash_mul;
      h ^= (uint8_t)(id >> 16); h *= hash_mul;
      h ^= (uint8_t)(id >> 24); h *= hash_mul;
      // ts, LSB first
      h ^= (uint8_t)t;          h *= hash_mul;
      h ^= (uint8_t)(t >> 8);   h *= hash_mul;
      h ^= (uint8_t)(t >> 16);  h *= hash_mul;
      h ^= (uint8_t)(t >> 24);  h *= hash_mul;
      // level, LSB first
      h ^= (uint8_t)level;      h *= hash_mul;
      h ^= (uint8_t)(level >> 8); h *= hash_mul;
      // len, LSB first
      h ^= (uint8_t)len;        h *= hash_mul;
      h ^= (uint8_t)(len >> 8); h *= hash_mul;
    }
    final_hash = h;
  }
  return final_hash;
}