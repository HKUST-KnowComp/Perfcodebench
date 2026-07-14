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

void heapify(Pair* arr, int n, int i, int k) {
  int smallest = i;
  int left = 2 * i + 1;
  int right = 2 * i + 2;
  if (left < n && left < k && (arr[left].score < arr[smallest].score || (arr[left].score == arr[smallest].score && arr[left].index > arr[smallest].index))) {
    smallest = left;
  }
  if (right < n && right < k && (arr[right].score < arr[smallest].score || (arr[right].score == arr[smallest].score && arr[right].index > arr[smallest].index))) {
    smallest = right;
  }
  if (smallest != i) {
    Pair temp = arr[i];
    arr[i] = arr[smallest];
    arr[smallest] = temp;
    heapify(arr, n, smallest, k);
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
    for (int i = (n / 2) - 1; i >= 0; --i) {
      heapify(pairs, n, i, n);
    }
    for (int i = n - 1; i >= n - k; --i) {
      Pair temp = pairs[0];
      pairs[0] = pairs[i];
      pairs[i] = temp;
      heapify(pairs, i, 0, i);
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