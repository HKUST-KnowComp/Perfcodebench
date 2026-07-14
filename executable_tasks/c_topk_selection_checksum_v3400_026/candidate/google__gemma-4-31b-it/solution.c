#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const Pair* a, const Pair* b) {
  if (a->score != b->score) return (a->score > b->score) ? -1 : 1;
  if (a->index != b->index) return (a->index < b->index) ? -1 : 1;
  return 0;
}

static void swap_pairs(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static void quickselect(Pair* arr, size_t left, size_t right, size_t k) {
  if (left >= right) return;
  
  size_t i = left;
  size_t j = right;
  Pair pivot = arr[(left + right) / 2];
  
  while (i <= j) {
    while (compare_pairs(&arr[i], &pivot) < 0) i++;
    while (compare_pairs(&arr[j], &pivot) > 0) j--;
    if (i <= j) {
      swap_pairs(&arr[i], &arr[j]);
      i++;
      if (j > 0) j--;
    }
  }

  if (k < j) quickselect(arr, left, j - 1, k);
  else if (k > i) quickselect(arr, i, right, k);
}

static int final_cmp(const void* lhs, const void* rhs) {
  return compare_pairs((const Pair*)lhs, (const Pair*)rhs);
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

    // Partial sort: find top k elements
    // We use a simple quickselect to partition the array such that 
    // the first k elements are the largest.
    size_t left = 0, right = n - 1;
    while (right - left > 0) {
      size_t i = left, j = right;
      Pair pivot = pairs[left + (right - left) / 2];
      while (i <= j) {
        while (compare_pairs(&pairs[i], &pivot) < 0) i++;
        while (compare_pairs(&pairs[j], &pivot) > 0) j--;
        if (i <= j) {
          swap_pairs(&pairs[i], &pairs[j]);
          i++;
          if (j > 0) j--;
        }
      }
      if ((int)i <= k) {
        left = i;
      } else {
        right = j;
      }
      if (left >= (size_t)k) break;
    }

    // The first k elements are now the top k, but not necessarily sorted.
    qsort(pairs, k, sizeof(Pair), final_cmp);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}