#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare(Pair a, Pair b) {
  if (a.score != b.score) return (a.score > b.score) ? -1 : 1;
  return (a.index < b.index) ? -1 : 1;
}

static void quickselect(Pair* arr, int left, int right, int k) {
  while (left < right) {
    int i = left, j = right;
    Pair pivot = arr[left + (right - left) / 2];
    while (i <= j) {
      while (compare(arr[i], pivot) < 0) i++;
      while (compare(arr[j], pivot) > 0) j--;
      if (i <= j) {
        Pair tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        i++; j--;
      }
    }
    if (k <= j) right = j;
    else if (k >= i) left = i;
    else break;
  }
}

static int cmp_pair_desc(const void* lhs, const void* rhs) {
  return compare(*(const Pair*)lhs, *(const Pair*)rhs);
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
    if (k > 0) {
      quickselect(pairs, 0, n - 1, k - 1);
      qsort(pairs, k, sizeof(Pair), cmp_pair_desc);
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