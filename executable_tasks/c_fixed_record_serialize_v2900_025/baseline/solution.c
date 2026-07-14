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

static void push_byte(uint8_t** out, size_t* size, size_t* cap, uint8_t b) {
  if (*size == *cap) {
    size_t next = (*cap == 0u) ? 64u : (*cap * 2u);
    uint8_t* grown = (uint8_t*)realloc(*out, next);
    if (!grown) exit(4);
    *out = grown;
    *cap = next;
  }
  (*out)[(*size)++] = b;
}

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters) {
  uint64_t hash = 0;
  uint8_t* out = NULL;
  size_t size = 0;
  size_t cap = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size = 0;
    for (size_t i = 0; i < count; ++i) {
      push_byte(&out, &size, &cap, (uint8_t)ids[i]);
      push_byte(&out, &size, &cap, (uint8_t)(ids[i] >> 8));
      push_byte(&out, &size, &cap, (uint8_t)(ids[i] >> 16));
      push_byte(&out, &size, &cap, (uint8_t)(ids[i] >> 24));
      push_byte(&out, &size, &cap, (uint8_t)ts[i]);
      push_byte(&out, &size, &cap, (uint8_t)(ts[i] >> 8));
      push_byte(&out, &size, &cap, (uint8_t)(ts[i] >> 16));
      push_byte(&out, &size, &cap, (uint8_t)(ts[i] >> 24));
      push_byte(&out, &size, &cap, (uint8_t)levels[i]);
      push_byte(&out, &size, &cap, (uint8_t)(levels[i] >> 8));
      push_byte(&out, &size, &cap, (uint8_t)lens[i]);
      push_byte(&out, &size, &cap, (uint8_t)(lens[i] >> 8));
    }
    hash = checksum_bytes(out, size);
  }
  free(out);
  return hash;
}
