#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int cmp_pair_desc(const Pair* a, const Pair* b) {
  if (a->score != b->score) {
    return (a->score < b->score) - (a->score > b->score);
  }
  return (a->index > b->index) - (a->index < b->index);
}

static inline void swap_pair(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static size_t partition(Pair* arr, size_t left, size_t right) {
  size_t mid = left + (right - left) / 2;
  if (cmp_pair_desc(&arr[mid], &arr[left]) < 0) swap_pair(&arr[left], &arr[mid]);
  if (cmp_pair_desc(&arr[right], &arr[left]) < 0) swap_pair(&arr[left], &arr[right]);
  if (cmp_pair_desc(&arr[mid], &arr[right]) < 0) swap_pair(&arr[mid], &arr[right]);
  
  Pair pivot = arr[right];
  size_t i = left;
  
  for (size_t j = left; j < right; ++j) {
    if (cmp_pair_desc(&arr[j], &pivot) < 0) {
      if (i != j) swap_pair(&arr[i], &arr[j]);
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

static void insertion_sort(Pair* arr, size_t n) {
  for (size_t i = 1; i < n; ++i) {
    Pair key = arr[i];
    size_t j = i;
    while (j > 0 && cmp_pair_desc(&arr[j - 1], &key) > 0) {
      arr[j] = arr[j - 1];
      --j;
    }
    arr[j] = key;
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
    
    if ((size_t)k < n) {
      quickselect(pairs, 0, n - 1, (size_t)k - 1);
      insertion_sort(pairs, (size_t)k);
    } else {
      insertion_sort(pairs, n);
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