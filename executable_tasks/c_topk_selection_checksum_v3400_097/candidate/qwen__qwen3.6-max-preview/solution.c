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

static inline void sift_down(Pair* arr, int root, int end) {
    while (1) {
        int child = 2 * root + 1;
        if (child > end) break;
        int swap = root;
        if (arr[child].score < arr[swap].score || (arr[child].score == arr[swap].score && arr[child].index > arr[swap].index))
            swap = child;
        if (child + 1 <= end) {
            if (arr[child+1].score < arr[swap].score || (arr[child+1].score == arr[swap].score && arr[child+1].index > arr[swap].index))
                swap = child + 1;
        }
        if (swap == root) break;
        Pair tmp = arr[root]; arr[root] = arr[swap]; arr[swap] = tmp;
        root = swap;
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (k <= 0) return 0;
  if ((size_t)k > n) k = (int)n;

  Pair* topk = (Pair*)malloc(k * sizeof(Pair));
  if (!topk) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < k; ++i) {
      topk[i].score = scores[i];
      topk[i].index = (uint32_t)i;
    }
    for (int i = k / 2 - 1; i >= 0; --i) {
      sift_down(topk, i, k - 1);
    }

    for (size_t i = k; i < n; ++i) {
      uint32_t s = scores[i];
      uint32_t idx = (uint32_t)i;
      if (s > topk[0].score || (s == topk[0].score && idx < topk[0].index)) {
        topk[0].score = s;
        topk[0].index = idx;
        sift_down(topk, 0, k - 1);
      }
    }

    qsort(topk, k, sizeof(Pair), cmp_pair_desc);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)topk[i].score << 32) ^ topk[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(topk);
  return h;
}
