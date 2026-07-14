#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const Pair* a, const Pair* b) {
  if (a->score != b->score) return (a->score < b->score) ? 1 : -1;
  if (a->index != b->index) return (a->index < b->index) ? -1 : 1;
  return 0;
}

static void swap_pairs(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static void quick_select(Pair* pairs, int left, int right, int k) {
  while (left < right) {
    Pair pivot = pairs[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
      while (compare_pairs(&pairs[i], &pivot) < 0) i++;
      while (compare_pairs(&pairs[j], &pivot) > 0) j--;
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

static int final_cmp(const void* a, const void* b) {
  return compare_pairs((const Pair*)a, (const Pair*)b);
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
      quick_select(pairs, 0, (int)n - 1, k - 1);
      qsort(pairs, k, sizeof(Pair), final_cmp);
    } else {
      qsort(pairs, n, sizeof(Pair), final_cmp);
    }

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}