#include "interface.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return (uint8_t)(((uint32_t)s * (uint32_t)a + (uint32_t)d * (uint32_t)(255u - a) + 127u) / 255u);
}

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

static uint64_t expected_checksum(const uint8_t* src, const uint8_t* dst, int width, int height) {
  size_t pixels = (size_t)width * (size_t)height;
  uint8_t* out = (uint8_t*)malloc(pixels * 4u);
  if (!out) exit(3);
  for (size_t i = 0; i < pixels; ++i) {
    size_t idx = i * 4u;
    uint8_t a = src[idx + 3u];
    out[idx] = blend_byte(src[idx], dst[idx], a);
    out[idx + 1u] = blend_byte(src[idx + 1u], dst[idx + 1u], a);
    out[idx + 2u] = blend_byte(src[idx + 2u], dst[idx + 2u], a);
    out[idx + 3u] = 255u;
  }
  uint64_t hash = checksum_bytes(out, pixels * 4u);
  free(out);
  return hash;
}

int main(void) {
  const int width = 144;
  const int height = 200;
  const int iters = 34;
  const size_t pixels = (size_t)width * (size_t)height;
  uint8_t* src = (uint8_t*)malloc(pixels * 4u);
  uint8_t* dst = (uint8_t*)malloc(pixels * 4u);
  if (!src || !dst) return 2;
  uint64_t state = 0x123456789ABCDEF0ULL;
  for (size_t i = 0; i < pixels; ++i) {
    for (int c = 0; c < 3; ++c) {
      state = state * 6364136223846793005ULL + 1442695040888963407ULL;
      src[i * 4u + (size_t)c] = (uint8_t)(state >> 56);
      state = state * 2862933555777941757ULL + 3037000493ULL;
      dst[i * 4u + (size_t)c] = (uint8_t)(state >> 56);
    }
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    src[i * 4u + 3u] = (uint8_t)(state >> 56);
    dst[i * 4u + 3u] = 255u;
  }
  uint64_t expected = expected_checksum(src, dst, width, height);
  clock_t start = clock();
  uint64_t actual = run(src, dst, width, height, iters);
  clock_t end = clock();
  long long elapsed_ns = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = (actual == expected);
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
         ok ? "true" : "false",
         elapsed_ns,
         expected,
         actual);
  free(src);
  free(dst);
  return ok ? 0 : 1;
}
