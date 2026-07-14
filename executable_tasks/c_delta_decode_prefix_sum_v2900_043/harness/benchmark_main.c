#include "interface.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static uint64_t expected_sum(const int32_t* deltas, size_t count) {
  int64_t current = 0;
  uint64_t total = 0;
  for (size_t i = 0; i < count; ++i) {
    current += (int64_t)deltas[i];
    total += (uint64_t)(current + 0x9E3779B1LL);
  }
  return total;
}

int main(void) {
  const size_t count = 175700u;
  const int iters = 36;
  int32_t* deltas = (int32_t*)malloc(count * sizeof(int32_t));
  if (!deltas) return 2;
  uint64_t state = 0xCAFEBABE13572468ULL;
  for (size_t i = 0; i < count; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    deltas[i] = (int32_t)((int32_t)((state >> 24) & 1023u) - 299);
  }
  uint64_t expected = expected_sum(deltas, count);
  clock_t start = clock();
  uint64_t actual = run(deltas, count, iters);
  clock_t end = clock();
  long long elapsed_ns = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = (actual == expected);
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
         ok ? "true" : "false", elapsed_ns, expected, actual);
  free(deltas);
  return ok ? 0 : 1;
}
