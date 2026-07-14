#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

static inline int pair_gt(const Pair* a, const Pair* b) {
    if (a->score != b->score) return a->score > b->score;
    return a->index < b->index;
}

static inline void swap_pair(Pair* a, Pair* b) {
    Pair t = *a; *a = *b; *b = t;
}

static void quickselect_topk(Pair* arr, size_t n, int k) {
    size_t left = 0;
    size_t right = n - 1;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (!pair_gt(&arr[left], &arr[mid])) swap_pair(&arr[left], &arr[mid]);
        if (!pair_gt(&arr[left], &arr[right])) swap_pair(&arr[left], &arr[right]);
        if (!pair_gt(&arr[mid], &arr[right])) swap_pair(&arr[mid], &arr[right]);
        swap_pair(&arr[mid], &arr[right]);
        
        Pair pivot = arr[right];
        size_t i = left;
        for (size_t j = left; j < right; ++j) {
            if (pair_gt(&arr[j], &pivot)) {
                swap_pair(&arr[i], &arr[j]);
                i++;
            }
        }
        swap_pair(&arr[i], &arr[right]);
        
        if (i >= (size_t)k) {
            right = i - 1;
        } else if (i < (size_t)k - 1) {
            left = i + 1;
        } else {
            return;
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
    
    if (k < (int)n) {
        quickselect_topk(pairs, n, k);
    }
    qsort(pairs, (size_t)k, sizeof(Pair), cmp_pair_desc);
    
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}