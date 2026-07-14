#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  Pair* pairs = (Pair*)malloc((k + 1) * sizeof(Pair));
  if (!pairs) return 0;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t m = 0;
    for (size_t i = 0; i < n; ++i) {
      uint32_t s = scores[i];
      if (m < (size_t)k || s > pairs[k - 1].score || 
          (s == pairs[k - 1].score && (uint32_t)i < pairs[k - 1].index)) {
        size_t j = (m < (size_t)k) ? m : k - 1;
        pairs[j].score = s;
        pairs[j].index = (uint32_t)i;
        if (m < (size_t)k) ++m;
        if (m == (size_t)k) {
          qsort(pairs, k, sizeof(Pair), cmp_pair_desc);
        }
      }
    }
    if (m < (size_t)k) qsort(pairs, m, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}