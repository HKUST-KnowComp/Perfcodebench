#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int is_better(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score > b->score;
  return a->index < b->index;
}

static void swap_pairs(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static void quickselect(Pair* pairs, int left, int right, int k) {
  while (left < right) {
    Pair pivot = pairs[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
      while (is_better(&pairs[i], &pivot)) i++;
      while (is_better(&pivot, &pairs[j])) j--;
      if (i <= j) {
        swap_pairs(&pairs[i], &pairs[j]);
        i++;
        j--;
      }
    }
    if (k <= j) right = j;
    else if (k >= i) left = i;
    else break;
  }
}

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score != b->score) return (a->score < b->score) ? 1 : -1;
  return (a->index < b->index) ? -1 : 1;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k == 0) return 0;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }

    if (k < (int)n) {
      quickselect(pairs, 0, (int)n - 1, k - 1);
    }
    
    qsort(pairs, k, sizeof(Pair), cmp_pair_desc);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}