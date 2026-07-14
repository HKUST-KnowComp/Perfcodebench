#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t hash = 0;
  size_t total_size = count * 12;
  uint8_t* out = (uint8_t*)malloc(total_size);
  if (!out) exit(4);

  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    size_t idx = 0;
    for (size_t i = 0; i < count; ++i) {
      out[idx] = (uint8_t)ids[i];
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)(ids[i] >> 8);
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)(ids[i] >> 16);
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)(ids[i] >> 24);
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)ts[i];
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)(ts[i] >> 8);
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)(ts[i] >> 16);
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)(ts[i] >> 24);
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)levels[i];
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)(levels[i] >> 8);
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)lens[i];
      hash ^= out[idx++];
      hash *= 1099511628211ULL;

      out[idx] = (uint8_t)(lens[i] >> 8);
      hash ^= out[idx++];
      hash *= 1099511628211ULL;
    }
  }

  free(out);
  return hash;
}