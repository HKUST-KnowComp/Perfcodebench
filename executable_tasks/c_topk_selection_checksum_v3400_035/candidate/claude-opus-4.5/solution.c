#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

static size_t partition(Pair* arr, size_t lo, size_t hi) {
  size_t mid = lo + (hi - lo) / 2;
  if (pair_gt(&arr[mid], &arr[lo])) swap_pair(&arr[lo], &arr[mid]);
  if (pair_gt(&arr[hi], &arr[lo])) swap_pair(&arr[lo], &arr[hi]);
  if (pair_gt(&arr[mid], &arr[hi])) swap_pair(&arr[mid], &arr[hi]);
  Pair pivot = arr[hi];
  size_t i = lo;
  for (size_t j = lo; j < hi; ++j) {
    if (pair_gt(&arr[j], &pivot) || 
        (arr[j].score == pivot.score && arr[j].index < pivot.index)) {
      swap_pair(&arr[i], &arr[j]);
      ++i;
    }
  }
  swap_pair(&arr[i], &arr[hi]);
  return i;
}

static void quickselect(Pair* arr, size_t lo, size_t hi, size_t k) {
  while (lo < hi) {
    size_t p = partition(arr, lo, hi);
    if (p == k) return;
    if (p > k) {
      hi = p - 1;
    } else {
      lo = p + 1;
    }
  }
}

static void insertion_sort(Pair* arr, size_t n) {
  for (size_t i = 1; i < n; ++i) {
    Pair key = arr[i];
    size_t j = i;
    while (j > 0 && pair_gt(&key, &arr[j-1])) {
      arr[j] = arr[j-1];
      --j;
    }
    arr[j] = key;
  }
}

static void topk_sort(Pair* arr, size_t lo, size_t hi) {
  if (hi <= lo) return;
  size_t n = hi - lo + 1;
  if (n <= 32) {
    insertion_sort(arr + lo, n);
    return;
  }
  size_t p = partition(arr, lo, hi);
  if (p > lo) topk_sort(arr, lo, p - 1);
  if (p < hi) topk_sort(arr, p + 1, hi);
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0 || n == 0) return 0;
  size_t kk = (size_t)k;
  if (kk > n) kk = n;
  
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;
  
  uint64_t h = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }
    
    if (kk < n) {
      quickselect(pairs, 0, n - 1, kk - 1);
    }
    
    topk_sort(pairs, 0, kk - 1);
    
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < kk; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  
  free(pairs);
  return h;
}