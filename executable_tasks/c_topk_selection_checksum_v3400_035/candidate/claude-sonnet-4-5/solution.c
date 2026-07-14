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

static inline int pair_greater(const Pair* a, const Pair* b) {
  if (a->score > b->score) return 1;
  if (a->score < b->score) return 0;
  return a->index < b->index;
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

static size_t partition(Pair* arr, size_t left, size_t right) {
  size_t mid = left + (right - left) / 2;
  if (pair_greater(&arr[mid], &arr[left])) swap_pair(&arr[left], &arr[mid]);
  if (pair_greater(&arr[right], &arr[left])) swap_pair(&arr[left], &arr[right]);
  if (pair_greater(&arr[mid], &arr[right])) swap_pair(&arr[mid], &arr[right]);
  
  Pair pivot = arr[right];
  size_t i = left;
  
  for (size_t j = left; j < right; ++j) {
    if (pair_greater(&arr[j], &pivot)) {
      swap_pair(&arr[i], &arr[j]);
      ++i;
    }
  }
  swap_pair(&arr[i], &arr[right]);
  return i;
}

static void quickselect(Pair* arr, size_t left, size_t right, size_t k) {
  while (left < right) {
    size_t pivot_idx = partition(arr, left, right);
    if (pivot_idx == k) {
      return;
    } else if (pivot_idx > k) {
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
    
    if (k > 0 && (size_t)k < n) {
      quickselect(pairs, 0, n - 1, (size_t)(k - 1));
      qsort(pairs, k, sizeof(Pair), cmp_pair_desc);
    } else if (k > 0) {
      qsort(pairs, n, sizeof(Pair), cmp_pair_desc);
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