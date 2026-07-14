#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int pair_gt(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score > b->score;
  return a->index < b->index;
}

static inline void swap_pair(Pair* a, Pair* b) {
  Pair t = *a; *a = *b; *b = t;
}

static void partial_qselect(Pair* arr, size_t left, size_t right, size_t k) {
  while (left < right) {
    size_t mid = left + (right - left) / 2;
    if (pair_gt(&arr[left], &arr[mid])) swap_pair(&arr[left], &arr[mid]);
    if (pair_gt(&arr[left], &arr[right])) swap_pair(&arr[left], &arr[right]);
    if (pair_gt(&arr[mid], &arr[right])) swap_pair(&arr[mid], &arr[right]);
    swap_pair(&arr[mid], &arr[right - 1]);
    Pair pivot = arr[right - 1];
    size_t i = left, j = right - 1;
    for (;;) {
      while (pair_gt(&arr[++i], &pivot));
      while (pair_gt(&pivot, &arr[--j])) if (j == left) break;
      if (i >= j) break;
      swap_pair(&arr[i], &arr[j]);
    }
    swap_pair(&arr[i], &arr[right - 1]);
    if (i >= k) right = i > 0 ? i - 1 : 0;
    else left = i + 1;
  }
}

static void insertion_sort(Pair* arr, size_t n) {
  for (size_t i = 1; i < n; ++i) {
    Pair key = arr[i];
    size_t j = i;
    while (j > 0 && pair_gt(&key, &arr[j - 1])) {
      arr[j] = arr[j - 1];
      --j;
    }
    arr[j] = key;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  size_t kk = (size_t)k < n ? (size_t)k : n;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }
    if (kk < n) {
      partial_qselect(pairs, 0, n - 1, kk);
    }
    insertion_sort(pairs, kk);
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < kk; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}
