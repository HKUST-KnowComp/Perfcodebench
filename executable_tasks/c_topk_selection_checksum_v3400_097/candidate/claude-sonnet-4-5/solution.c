#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int pair_less(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score < b->score;
  return a->index > b->index;
}

static inline void swap_pair(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static void insertion_sort(Pair* arr, int n) {
  for (int i = 1; i < n; ++i) {
    Pair key = arr[i];
    int j = i - 1;
    while (j >= 0 && pair_less(&arr[j], &key)) {
      arr[j + 1] = arr[j];
      --j;
    }
    arr[j + 1] = key;
  }
}

static int partition(Pair* arr, int low, int high) {
  int mid = low + (high - low) / 2;
  if (pair_less(&arr[mid], &arr[low])) swap_pair(&arr[low], &arr[mid]);
  if (pair_less(&arr[high], &arr[low])) swap_pair(&arr[low], &arr[high]);
  if (pair_less(&arr[mid], &arr[high])) swap_pair(&arr[mid], &arr[high]);
  
  Pair pivot = arr[high];
  int i = low - 1;
  
  for (int j = low; j < high; ++j) {
    if (!pair_less(&arr[j], &pivot)) {
      ++i;
      swap_pair(&arr[i], &arr[j]);
    }
  }
  swap_pair(&arr[i + 1], &arr[high]);
  return i + 1;
}

static void quickselect(Pair* arr, int low, int high, int k) {
  while (low < high) {
    if (high - low < 16) {
      insertion_sort(arr + low, high - low + 1);
      return;
    }
    
    int pi = partition(arr, low, high);
    
    if (pi == k) {
      return;
    } else if (pi < k) {
      low = pi + 1;
    } else {
      high = pi - 1;
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
    
    if (k >= (int)n) {
      insertion_sort(pairs, n);
    } else {
      quickselect(pairs, 0, n - 1, k - 1);
      insertion_sort(pairs, k);
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