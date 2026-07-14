#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score != b->score) return (a->score < b->score) ? 1 : -1;
  return (a->index < b->index) ? -1 : 1;
}

static void swap_pair(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static void quickselect(Pair* arr, int left, int right, int k) {
  while (left < right) {
    int pivot_idx = left + (right - left) / 2;
    Pair pivot = arr[pivot_idx];
    swap_pair(&arr[pivot_idx], &arr[right]);
    int store = left;
    for (int i = left; i < right; i++) {
      // Equivalent to cmp_pair_desc(arr[i], pivot) < 0
      if (arr[i].score > pivot.score || (arr[i].score == pivot.score && arr[i].index < pivot.index)) {
        swap_pair(&arr[i], &arr[store]);
        store++;
      }
    }
    swap_pair(&arr[store], &arr[right]);
    if (store == k) return;
    if (store < k) left = store + 1;
    else right = store - 1;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  if (k > (int)n) k = (int)n;

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
      h ^= ((uint64_t)pairs[i].score << 32) ^ (uint64_t)pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}