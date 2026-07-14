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

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }
    // Use a heap to select top-k elements
    Pair* top_k = (Pair*)malloc(k * sizeof(Pair));
    if (!top_k) {
      free(pairs);
      return 0;
    }
    for (size_t i = 0; i < n; ++i) {
      if (i < k) {
        top_k[i] = pairs[i];
      } else {
        if (pairs[i].score > top_k[k-1].score || (pairs[i].score == top_k[k-1].score && pairs[i].index < top_k[k-1].index)) {
          top_k[k-1] = pairs[i];
          // Heapify down
          int j = k - 1;
          while (j > 0) {
            int parent = (j - 1) / 2;
            if (top_k[parent].score > top_k[j].score || (top_k[parent].score == top_k[j].score && top_k[parent].index > top_k[j].index)) {
              break;
            }
            Pair temp = top_k[parent];
            top_k[parent] = top_k[j];
            top_k[j] = temp;
            j = parent;
          }
        }
      }
    }
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)top_k[i].score << 32) ^ top_k[i].index;
      h *= 1099511628211ULL;
    }
    free(top_k);
  }
  free(pairs);
  return h;
}