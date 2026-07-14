#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const Pair a, const Pair b) {
  if (a.score != b.score) {
    return b.score - a.score; // Descending score
  }
  return (int)a.index - (int)b.index; // Ascending index
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

static void quickselect(Pair* arr, int n, int k) {
  int left = 0, right = n - 1;
  while (left < right) {
    int pivotIndex = left + (right - left) / 2;
    // Simple median-of-three or similar could be used, but standard partition is often enough
    // For stability in this specific task, we use a basic partition.
    int p = partition(arr, left, right);
    if (p == k) return;
    if (p < k) left = p + 1;
    else right = p - 1;
  }
}

// Standard qsort for the final k elements
static int qsort_cmp(const void* a, const void* b) {
  return compare_pairs(*(const Pair*)a, *(const Pair*)b);
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  if (k > (int)n) k = (int)n;

  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  // Pre-fill indices to avoid re-calculating inside the loop if scores don't change
  // However, the task implies scores might be the same across iters but we must follow the loop structure.
  // The baseline re-assigns scores[i] every iteration.
  
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }

    // Use Quickselect to get top k in O(n)
    quickselect(pairs, (int)n, k);

    // The top k elements are in pairs[0...k-1], but they are not necessarily sorted.
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