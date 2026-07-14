#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int cmp_pair_desc(const Pair a, const Pair b) {
  if (a.score > b.score) return -1;
  if (a.score < b.score) return 1;
  if (a.index < b.index) return -1;
  if (a.index > b.index) return 1;
  return 0;
}

static int qsort_cmp(const void* lhs, const void* rhs) {
  return cmp_pair_desc(*(const Pair*)lhs, *(const Pair*)rhs);
}

static void quickselect(Pair* arr, int left, int right, int k) {
  while (left < right) {
    int i = left, j = right;
    Pair pivot = arr[left + (right - left) / 2];
    while (i <= j) {
      while (cmp_pair_desc(arr[i], pivot) < 0) i++;
      while (cmp_pair_desc(arr[j], pivot) > 0) j--;
      if (i <= j) {
        Pair tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
        i++;
        j--;
      }
    }
    if (k <= j) right = j;
    else if (i <= k) left = i;
    else break;
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

    if (k < (int)n) {
      quickselect(pairs, 0, (int)n - 1, k - 1);
    }
    
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