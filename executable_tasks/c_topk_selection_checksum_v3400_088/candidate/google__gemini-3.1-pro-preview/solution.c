#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score != b->score) return (a->score > b->score) ? -1 : 1;
  return (a->index < b->index) ? -1 : 1;
}

static inline int is_better(Pair a, Pair b) {
  if (a.score != b.score) return a.score > b.score;
  return a.index < b.index;
}

static void quickselect(Pair* arr, int n, int k) {
  int left = 0, right = n - 1;
  while (left < right) {
    int mid = left + (right - left) / 2;
    Pair pivot = arr[mid];
    // Median-of-three
    if (is_better(arr[left], pivot)) {
        if (is_better(pivot, arr[right])) {} 
        else if (is_better(arr[left], arr[right])) pivot = arr[right];
        else pivot = arr[left];
    } else {
        if (is_better(arr[right], pivot)) {} 
        else if (is_better(arr[right], arr[left])) pivot = arr[right];
        else pivot = arr[left];
    }

    int i = left, j = right;
    while (i <= j) {
      while (is_better(arr[i], pivot)) i++;
      while (is_better(pivot, arr[j])) j--;
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
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }

    if (k < (int)n) {
      quickselect(pairs, (int)n, k - 1);
    }
    
    qsort(pairs, k, sizeof(Pair), cmp_pair_desc);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}
