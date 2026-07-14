#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static void swap(Pair* a, Pair* b) {
  Pair temp = *a;
  *a = *b;
  *b = temp;
}

static void heapify(Pair* arr, size_t n, size_t i) {
  size_t smallest = i;
  size_t left = 2 * i + 1;
  size_t right = 2 * i + 2;

  if (left < n && (arr[left].score > arr[smallest].score ||
      (arr[left].score == arr[smallest].score && arr[left].index < arr[smallest].index))) {
    smallest = left;
  }

  if (right < n && (arr[right].score > arr[smallest].score ||
      (arr[right].score == arr[smallest].score && arr[right].index < arr[smallest].index))) {
    smallest = right;
  }

  if (smallest != i) {
    swap(&arr[i], &arr[smallest]);
    heapify(arr, n, smallest);
  }
}

static void build_min_heap(Pair* arr, size_t n) {
  for (int i = n / 2 - 1; i >= 0; --i) {
    heapify(arr, n, i);
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
    if (k < (int)n) {
      build_min_heap(pairs, k);
      for (size_t i = k; i < n; ++i) {
        if (pairs[0].score < scores[i] || (pairs[0].score == scores[i] && pairs[0].index > i)) {
          pairs[0].score = scores[i];
          pairs[0].index = i;
          heapify(pairs, k, 0);
        }
      }
    } else {
      for (size_t i = 0; i < n; ++i) {
        pairs[i].score = scores[i];
        pairs[i].index = (uint32_t)i;
      }
      build_min_heap(pairs, n);
    }
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      Pair top = pairs[0];
      swap(&pairs[0], &pairs[k - i - 1]);
      heapify(pairs, k - i - 1, 0);
      h ^= ((uint64_t)top.score << 32) ^ top.index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}
