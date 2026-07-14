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

static void quickselect(Pair* arr, size_t n, size_t k) {
  size_t left = 0;
  size_t right = n - 1;
  while (left < right) {
    Pair pivot = arr[(left + right) / 2];
    size_t i = left;
    size_t j = right;
    while (i <= j) {
      while (compare_pairs(arr[i], pivot) < 0) i++;
      while (compare_pairs(arr[j], pivot) > 0) j--;
      if (i <= j) {
        swap(&arr[i], &arr[j]);
        i++;
        j--;
      }
    }
    if (k <= j) right = j;
    else if (i <= k) left = i;
    else break;
  }
}

// Standard quicksort for the final k elements to ensure exact order
static int qsort_cmp(const void* a, const void* b) {
  const Pair* pa = (const Pair*)a;
  const Pair* pb = (const Pair*)b;
  if (pa->score != pb->score) return (pb->score > pa->score) ? 1 : -1;
  return (pa->index > pb->index) ? 1 : -1;
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

    // Use quickselect to find the top k elements in O(N) average
    quickselect(pairs, n, (size_t)k - 1);

    // Sort only the top k elements to satisfy the checksum requirement
    qsort(pairs, (size_t)k, sizeof(Pair), qsort_cmp);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}