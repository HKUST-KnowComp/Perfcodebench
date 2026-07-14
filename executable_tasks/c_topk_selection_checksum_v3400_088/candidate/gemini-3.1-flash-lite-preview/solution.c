#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score != b->score) return (a->score < b->score) ? 1 : -1;
  return (a->index < b->index) ? -1 : 1;
}

static void swap(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static void nth_element(Pair* arr, size_t n, size_t k) {
  size_t left = 0, right = n - 1;
  while (left < right) {
    size_t pivot_idx = left + (right - left) / 2;
    Pair pivot = arr[pivot_idx];
    size_t i = left, j = right;
    while (i <= j) {
      while (cmp_pair_desc(&arr[i], &pivot) < 0) i++;
      while (cmp_pair_desc(&arr[j], &pivot) > 0) j--;
      if (i <= j) {
        swap(&arr[i], &arr[j]);
        i++; j--;
      }
    }
    if (k <= j) right = j;
    else if (i <= k) left = i;
    else break;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }
    if (k > 0) {
      if (k < (int)n) nth_element(pairs, n, k - 1);
      qsort(pairs, k, sizeof(Pair), cmp_pair_desc);
      h = 1469598103934665603ULL;
      for (int i = 0; i < k; ++i) {
        h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
        h *= 1099511628211ULL;
      }
    } else {
      h = 1469598103934665603ULL;
    }
  }
  free(pairs);
  return h;
}