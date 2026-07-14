#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint64_t run(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets, int iters);

static uint64_t expected_checksum(const uint32_t* keys, const uint32_t* vals, size_t n, uint32_t buckets) {
  uint64_t* hist = (uint64_t*)calloc(buckets, sizeof(uint64_t));
  if (!hist) exit(2);
  for (size_t i = 0; i < n; ++i) hist[keys[i]] += vals[i];
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t i = 0; i < buckets; ++i) {
    hash ^= (hist[i] + 1ULL);
    hash *= 1099511628211ULL;
  }
  free(hist);
  return hash;
}

int main(void) {
  const size_t n = 192600u;
  const uint32_t buckets = 768u;
  const int iters = 26;
  uint32_t* keys = (uint32_t*)malloc(n * sizeof(uint32_t));
  uint32_t* vals = (uint32_t*)malloc(n * sizeof(uint32_t));
  if (!keys || !vals) return 2;
  uint64_t state = 0xA55A5AA55A5AA55AULL;
  for (size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    keys[i] = (uint32_t)((state >> 16) % buckets);
    vals[i] = 1u + (uint32_t)((state >> 40) & 31u);
  }
  uint64_t expected = expected_checksum(keys, vals, n, buckets);
  clock_t start = clock();
  uint64_t actual = run(keys, vals, n, buckets, iters);
  clock_t end = clock();
  long long elapsed = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = actual == expected;
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
      ok ? "true" : "false", elapsed, expected, actual);
  free(keys);
  free(vals);
  return ok ? 0 : 1;
}
