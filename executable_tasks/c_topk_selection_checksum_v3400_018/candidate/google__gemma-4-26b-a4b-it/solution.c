#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const Pair a, const Pair b) {
  if (a.score != b.score) return (a.score > b.score) ? -1 : 1;
  return (a.index < b.index) ? -1 : 1;
}

static void swap_pairs(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static int partition(Pair* arr, int left, int right) {
  Pair pivot = arr[right];
  int i = left;
  for (int j = left; j < right; j++) {
    if (compare_pairs(arr[j], pivot) < 0) {
      swap_pairs(&arr[i], &arr[j]);
      i++;
    }
  }
  swap_pairs(&arr[i], &arr[right]);
  return i;
}

static void quickselect(Pair* arr, int n, int k) {
  int left = 0, right = n - 1;
  while (left < right) {
    int pivotIndex = partition(arr, left, right);
    if (pivotIndex == k) return;
    if (pivotIndex < k) left = pivotIndex + 1;
    else right = pivotIndex - 1;
  }
}

static int sort_cmp(const void* a, const void* b) {
  return compare_pairs(*(const Pair*)a, *(const Pair*)b);
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  if (k > (int)n) k = (int)n;

  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  for (size_t i = 0; i < n; ++i) {
    pairs[i].score = scores[i];
    pairs[i].index = (uint32_t)i;
  }

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Note: The original code re-assigns scores every iteration.
    // If scores are constant, we only do this once. 
    // To preserve exact behavior of the baseline:
    if (iter > 0) {
        for (size_t i = 0; i < n; ++i) {
            pairs[i].score = scores[i];
            pairs[i].index = (uint32_t)i;
        }
    }

    // Use quickselect to get top k elements in O(N) average
    quickselect(pairs, (int)n, k);
    
    // The problem requires the checksum over the ORDERED winners.
    // Quickselect doesn't guarantee order within the top k.
    qsort(pairs, k, sizeof(Pair), sort_cmp);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}