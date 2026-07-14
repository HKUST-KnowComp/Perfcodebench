#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

uint64_t run(const uint32_t* scores, size_t n, int k, int iters);

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static int better(Pair a, Pair b) {
  if (a.score != b.score) return a.score > b.score;
  return a.index < b.index;
}

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

static uint64_t checksum_topk(const uint32_t* scores, size_t n, int k) {
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) exit(2);
  for (size_t i = 0; i < n; ++i) {
    pairs[i].score = scores[i];
    pairs[i].index = (uint32_t)i;
  }
  qsort(pairs, n, sizeof(Pair), cmp_pair_desc);
  uint64_t h = 1469598103934665603ULL;
  for (int i = 0; i < k; ++i) {
    h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
    h *= 1099511628211ULL;
  }
  free(pairs);
  return h;
}

int main(void) {
  const size_t n = 236400u;
  const int k = 24;
  const int iters = 18;
  uint32_t* scores = (uint32_t*)malloc(n * sizeof(uint32_t));
  if (!scores) return 2;
  uint64_t state = 0x3141592653589793ULL;
  for (size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    scores[i] = (uint32_t)((state >> 16) & 0xFFFFF);
  }
  uint64_t expected = checksum_topk(scores, n, k);
  clock_t start = clock();
  uint64_t actual = run(scores, n, k, iters);
  clock_t end = clock();
  long long elapsed = (long long)(((end - start) * 1000000000.0) / CLOCKS_PER_SEC);
  int ok = actual == expected;
  printf("{\"ok\":%s,\"elapsed_ns\":%lld,\"expected_checksum\":%" PRIu64 ",\"actual_checksum\":%" PRIu64 "}\n",
      ok ? "true" : "false", elapsed, expected, actual);
  free(scores);
  return ok ? 0 : 1;
}
