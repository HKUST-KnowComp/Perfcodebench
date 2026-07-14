#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const void* a, const void* b) {
  const Pair* p1 = (const Pair*)a;
  const Pair* p2 = (const Pair*)b;
  if (p1->score != p2->score) {
    return (p1->score > p2->score) ? -1 : 1;
  }
  return (p1->index < p2->index) ? -1 : 1;
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
    // Descending order logic for top-k
    if (arr[j].score > pivot.score || (arr[j].score == pivot.score && arr[j].index < pivot.index)) {
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
    int pivotIndex = partition(arr, left, right);
    if (pivotIndex == k) return;
    if (pivotIndex < k) {
      left = pivotIndex + 1;
    } else {
      right = pivotIndex - 1;
    }
  }
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

    // Use quickselect to bring top k elements to the front in O(N)
    quickselect(pairs, (int)n, k);

    // The top k elements are now in pairs[0...k-1], but they are not necessarily sorted.
    // The checksum requires the ordered winners.
    qsort(pairs, k, sizeof(Pair), compare_pairs);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}