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

static void swap(Pair* a, Pair* b) {
  Pair temp = *a;
  *a = *b;
  *b = temp;
}

static Pair* partition(Pair* arr, int low, int high) {
  Pair pivot = arr[high];
  int i = low - 1;
  for (int j = low; j < high; ++j) {
    if (cmp_pair_desc(&arr[j], &pivot) <= 0) {
      ++i;
      swap(&arr[i], &arr[j]);
    }
  }
  swap(&arr[i + 1], &arr[high]);
  return &arr[i + 1];
}

static void quickselect(Pair* arr, int low, int high, int k) {
  while (low < high) {
    Pair* pivot = partition(arr, low, high);
    int pivot_index = pivot - arr;
    if (pivot_index == k) return;
    if (k < pivot_index) high = pivot_index - 1;
    else low = pivot_index + 1;
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
    quickselect(pairs, 0, n - 1, k - 1);
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}