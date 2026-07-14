#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const Pair a, const Pair b) {
  if (a.score != b.score) return (a.score > b.score) ? -1 : 1;
  if (a.index != b.index) return (a.index < b.index) ? -1 : 1;
  return 0;
}

static void swap(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static int partition(Pair* arr, int left, int right) {
  Pair pivot = arr[right];
  int i = left;
  for (int j = left; j < right; j++) {
    if (compare_pairs(arr[j], pivot) < 0) {
      swap(&arr[i], &arr[j]);
      i++;
    }
  }
  swap(&arr[i], &arr[right]);
  return i;
}

static void quickselect(Pair* arr, int left, int right, int k) {
  while (left < right) {
    int pivotIndex = partition(arr, left, right);
    if (pivotIndex == k) return;
    if (pivotIndex < k) left = pivotIndex + 1;
    else right = pivotIndex - 1;
  }
}

static int qsort_cmp(const void* a, const void* b) {
  return compare_pairs(*(const Pair*)a, *(const Pair*)b);
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

    // Use quickselect to find the top-k elements in O(n) average time
    quickselect(pairs, 0, (int)n - 1, k - 1);

    // The top-k elements are now in indices [0, k-1], but they are not necessarily sorted.
    // The checksum requires them to be ordered.
    qsort(pairs, k, sizeof(Pair), qsort_cmp);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}