#include "interface.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint64_t checksum_bytes(const uint8_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint64_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

static void store_u32(uint8_t* dst, uint32_t v) {
  dst[0] = (uint8_t)v;
  dst[1] = (uint8_t)(v >> 8);
  dst[2] = (uint8_t)(v >> 16);
  dst[3] = (uint8_t)(v >> 24);
}

static void store_u16(uint8_t* dst, uint16_t v) {
  dst[0] = (uint8_t)v;
  dst[1] = (uint8_t)(v >> 8);
}

static uint64_t expected_checksum(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count) {
  uint8_t* out = (uint8_t*)malloc(count * 12u);
  if (!out) exit(3);
  uint8_t* ptr = out;
  for (size_t i = 0; i < count; ++i) {
    store_u32(ptr, ids[i]); ptr += 4;
    store_u32(ptr, ts[i]); ptr += 4;
    store_u16(ptr, levels[i]); ptr += 2;
    store_u16(ptr, lens[i]); ptr += 2;
  }
  uint64_t hash = checksum_bytes(out, count * 12u);
  free(out);
  return hash;
}

int main(void) {
  const size_t count = 357800u;
  const int iters = 32;
  uint32_t* ids = (uint32_t*)malloc(count * sizeof(uint32_t));
  uint32_t* ts = (uint32_t*)malloc(count * sizeof(uint32_t));
  uint16_t* levels = (uint16_t*)malloc(count * sizeof(uint16_t));
  uint16_t* lens = (uint16_t*)malloc(count * sizeof(uint16_t));
  if (!ids || !ts || !levels || !lens) return 2;
  uint64_t state = 0x13579BDF2468ACE0ULL;
  for (size_t i = 0; i < count; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    ids[i] = (uint32_t)(state & 0xFFFFFFFFu);
    state = state * 2862933555777941757ULL + 3037000493ULL;
    ts[i] = (uint32_t)(state & 0xFFFFFFFFu);
    levels[i] = (uint16_t)((state >> 16) & 7u);
    lens[i] = (uint16_t)(24u + ((state >> 24) & 255u));
  }
  uint64_t expected = expected_checksum(ids, ts, levels, lens, count);
  clock_t start = clock();
  uint64_t actual = run(ids, ts, levels, lens, count, iters);
  clock_t end = clock();
  long long elapsed_ns = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = (actual == expected);
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
         ok ? "true" : "false", elapsed_ns, expected, actual);
  free(ids);
  free(ts);
  free(levels);
  free(lens);
  return ok ? 0 : 1;
}
