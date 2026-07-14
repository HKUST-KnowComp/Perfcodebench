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

static inline int is_better(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score > b->score;
  return a->index < b->index;
}

static void sift_down(Pair* heap, int heap_size, int root) {
  int smallest = root;
  int left = 2 * root + 1;
  int right = 2 * root + 2;
  if (left < heap_size && !is_better(&heap[smallest], &heap[left])) {
    smallest = left;
  }
  if (right < heap_size && !is_better(&heap[smallest], &heap[right])) {
    smallest = right;
  }
  if (smallest != root) {
    Pair temp = heap[root];
    heap[root] = heap[smallest];
    heap[smallest] = temp;
    sift_down(heap, heap_size, smallest);
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;
  Pair* top_k = NULL;
  int use_heap = (k > 0 && k < (int)n);
  if (use_heap) {
    top_k = (Pair*)malloc(k * sizeof(Pair));
    if (!top_k) {
      free(pairs);
      return 0;
    }
  }
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }
    if (use_heap) {
      for (int i = 0; i < k; i++) {
        top_k[i] = pairs[i];
      }
      for (int i = (k / 2) - 1; i >= 0; i--) {
        sift_down(top_k, k, i);
      }
      for (size_t i = k; i < n; i++) {
        if (is_better(&pairs[i], &top_k[0])) {
          top_k[0] = pairs[i];
          sift_down(top_k, k, 0);
        }
      }
      qsort(top_k, k, sizeof(Pair), cmp_pair_desc);
      h = 1469598103934665603ULL;
      for (int i = 0; i < k; i++) {
        h ^= ((uint64_t)top_k[i].score << 32) ^ top_k[i].index;
        h *= 1099511628211ULL;
      }
    } else {
      qsort(pairs, n, sizeof(Pair), cmp_pair_desc);
      h = 1469598103934665603ULL;
      int limit = k < (int)n ? k : (int)n;
      for (int i = 0; i < limit; i++) {
        h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
        h *= 1099511628211ULL;
      }
    }
  }
  free(pairs);
  if (top_k) free(top_k);
  return h;
}