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

void heapify(Pair* arr, size_t n, size_t i, int k) {
  size_t largest = i;
  size_t left = 2 * i + 1;
  size_t right = 2 * i + 2;
  if (left < n && cmp_pair_desc(arr + left, arr + largest) < 0) largest = left;
  if (right < n && cmp_pair_desc(arr + right, arr + largest) < 0) largest = right;
  if (largest != i) {
    Pair temp = arr[i];
    arr[i] = arr[largest];
    arr[largest] = temp;
    heapify(arr, n, largest, k);
  }
}

void build_heap(Pair* arr, size_t n) {
  for (size_t i = n / 2; i >= 0; --i) heapify(arr, n, i, 0);
}

void heap_sort_top_k(Pair* arr, size_t n, int k) {
  build_heap(arr, n);
  for (int i = 0; i < k; ++i) {
    Pair temp = arr[0];
    arr[0] = arr[n - i - 1];
    arr[n - i - 1] = temp;
    heapify(arr, n - i - 1, 0, k);
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
    heap_sort_top_k(pairs, n, k);
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}