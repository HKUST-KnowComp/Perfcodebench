#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const Pair a, const Pair b) {
  if (a.score != b.score) return (a.score > b.score) ? -1 : 1;
  if (a.index != b.index) return (a.index < b.index) ? -1 : 1;
  return 0;
}

static void swap(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static int partition(Pair* arr, int left, int right, int pivot_idx) {
  Pair pivot_val = arr[pivot_idx];
  swap(&arr[pivot_idx], &arr[right]);
  int store_idx = left;
  for (int i = left; i < right; i++) {
    if (compare_pairs(arr[i], pivot_val) < 0) {
      swap(&arr[i], &arr[store_idx]);
      store_idx++;
    }
  }
  swap(&arr[store_idx], &arr[right]);
  return store_idx;
}

static void quickselect(Pair* arr, int n, int k) {
  int left = 0, right = n - 1;
  while (left < right) {
    int pivot_idx = left + (right - left) / 2;
    pivot_idx = partition(arr, left, right, pivot_idx);
    if (pivot_idx == k) return;
    if (pivot_idx < k) left = pivot_idx + 1;
    else right = pivot_idx - 1;
  }
}

static int sort_cmp(const void* a, const void* b) {
  return compare_pairs(*(const Pair*)a, *(const Pair*)b);
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  for (size_t i = 0; i < n; ++i) {
    pairs[i].score = scores[i];
    pairs[i].index = (uint32_t)i;
  }

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // We need to reset the pairs array if scores change, but here scores are constant
    // and only the iteration loop exists. However, the problem implies we must 
    // re-run the logic. Since scores don't change, we only need to re-copy if 
    // the selection/sort modifies the array. 
    // To be safe and correct for any potential score changes (though not in this snippet):
    for (size_t i = 0; i < n; ++i) {
        pairs[i].score = scores[i];
        pairs[i].index = (uint32_t)i;
    }

    if ((int)n > k) {
      quickselect(pairs, (int)n, k);
    }
    
    qsort(pairs, k, sizeof(Pair), sort_cmp);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}