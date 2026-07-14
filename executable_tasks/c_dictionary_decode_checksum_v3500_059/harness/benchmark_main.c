#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint64_t run(const uint32_t* ids, size_t n, const uint32_t* dict_vals, uint32_t dict_size, int iters);

static uint64_t expected_checksum(const uint32_t* ids, size_t n, const uint32_t* dict_vals) {
  uint64_t h = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    uint64_t v = dict_vals[ids[i]];
    h ^= v + 1ULL;
    h *= 1099511628211ULL;
  }
  return h;
}

int main(void) {
  const size_t n = 291300u;
  const uint32_t dict_size = 1664u;
  const int iters = 35;
  uint32_t* ids = (uint32_t*)malloc(n * sizeof(uint32_t));
  uint32_t* dict_vals = (uint32_t*)malloc((size_t)dict_size * sizeof(uint32_t));
  if (!ids || !dict_vals) return 2;
  uint64_t state = 0x9E3779B97F4A7C15ULL;
  for (uint32_t i = 0; i < dict_size; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    dict_vals[i] = (uint32_t)((state >> 16) & 0xFFFFFFu);
  }
  for (size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    ids[i] = (uint32_t)((state >> 20) % dict_size);
  }
  uint64_t expected = expected_checksum(ids, n, dict_vals);
  clock_t start = clock();
  uint64_t actual = run(ids, n, dict_vals, dict_size, iters);
  clock_t end = clock();
  long long elapsed = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = actual == expected;
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
      ok ? "true" : "false", elapsed, expected, actual);
  free(ids);
  free(dict_vals);
  return ok ? 0 : 1;
}
