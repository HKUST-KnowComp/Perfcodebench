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

static inline int cmp_pair_desc(const Pair* a, const Pair* b) {
  if (a->score != b->score) {
    return (a->score < b->score) ? 1 : -1;
  }
  return (a->index < b->index) ? -1 : (a->index > b->index);
}

static int cmp_pair_desc_qsort(const void* lhs, const void* rhs) {
  return cmp_pair_desc((const Pair*)lhs, (const Pair*)rhs);
}

static size_t partition(Pair* arr, size_t left, size_t right) {
  size_t mid = left + (right - left) / 2;
  Pair pivot = arr[mid];
  swap_pair(&arr[mid], &arr[right]);
  
  size_t store = left;
  for (size_t i = left; i < right; ++i) {
    if (cmp_pair_desc(&arr[i], &pivot) < 0) {
      if (i != store) {
        swap_pair(&arr[i], &arr[store]);
      }
      ++store;
    }
  }
  swap_pair(&arr[store], &arr[right]);
  return store;
}

static void quickselect(Pair* arr, size_t left, size_t right, size_t k) {
  while (left < right) {
    size_t pivot_idx = partition(arr, left, right);
    if (pivot_idx == k) {
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
  const size_t uk = (size_t)k;
  
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }
    
    if (uk < n) {
      quickselect(pairs, 0, n - 1, uk);
      qsort(pairs, uk, sizeof(Pair), cmp_pair_desc_qsort);
    } else {
      qsort(pairs, n, sizeof(Pair), cmp_pair_desc_qsort);
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