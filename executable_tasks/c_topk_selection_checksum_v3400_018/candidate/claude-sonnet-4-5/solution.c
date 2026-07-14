#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline void swap_pair(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static inline int pair_less(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score < b->score;
  return a->index > b->index;
}

static void insertion_sort(Pair* arr, int left, int right) {
  for (int i = left + 1; i <= right; ++i) {
    Pair key = arr[i];
    int j = i - 1;
    while (j >= left && pair_less(&arr[j], &key)) {
      arr[j + 1] = arr[j];
      --j;
    }
    arr[j + 1] = key;
  }
}

static int partition(Pair* arr, int left, int right, int pivot_idx) {
  Pair pivot = arr[pivot_idx];
  swap_pair(&arr[pivot_idx], &arr[right]);
  int store_idx = left;
  for (int i = left; i < right; ++i) {
    if (!pair_less(&arr[i], &pivot)) {
      swap_pair(&arr[i], &arr[store_idx]);
      ++store_idx;
    }
  }
  swap_pair(&arr[store_idx], &arr[right]);
  return store_idx;
}

static void quickselect(Pair* arr, int left, int right, int k) {
  while (right > left) {
    if (right - left < 16) {
      insertion_sort(arr, left, right);
      return;
    }
    int pivot_idx = left + (right - left) / 2;
    pivot_idx = partition(arr, left, right, pivot_idx);
    if (k == pivot_idx) {
      return;
    } else if (k < pivot_idx) {
      right = pivot_idx - 1;
    } else {
      left = pivot_idx + 1;
    }
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
    quickselect(pairs, 0, (int)n - 1, k - 1);
    insertion_sort(pairs, 0, k - 1);
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}
