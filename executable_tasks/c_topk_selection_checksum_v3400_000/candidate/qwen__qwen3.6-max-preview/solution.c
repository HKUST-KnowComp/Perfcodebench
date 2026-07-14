#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int is_worse(Pair a, Pair b) {
  return (a.score < b.score) || (a.score == b.score && a.index > b.index);
}

static void sift_down(Pair* heap, size_t size, size_t i) {
  while (1) {
    size_t left = 2 * i + 1;
    size_t right = 2 * i + 2;
    size_t worst = i;
    if (left < size && is_worse(heap[left], heap[worst])) worst = left;
    if (right < size && is_worse(heap[right], heap[worst])) worst = right;
    if (worst != i) {
      Pair tmp = heap[i];
      heap[i] = heap[worst];
      heap[worst] = tmp;
      i = worst;
    } else {
      break;
    }
  }
}

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
  if (k <= 0) return 0;
  size_t k_sz = (size_t)k;
  if (k_sz > n) k_sz = n;

  Pair* heap = (Pair*)malloc(k_sz * sizeof(Pair));
  if (!heap) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < k_sz; ++i) {
      heap[i].score = scores[i];
      heap[i].index = (uint32_t)i;
    }
    for (size_t i = k_sz / 2; i > 0; --i) {
      sift_down(heap, k_sz, i - 1);
    }
    for (size_t i = k_sz; i < n; ++i) {
      Pair curr;
      curr.score = scores[i];
      curr.index = (uint32_t)i;
      if (is_worse(heap[0], curr)) {
        heap[0] = curr;
        sift_down(heap, k_sz, 0);
      }
    }
    qsort(heap, k_sz, sizeof(Pair), cmp_pair_desc);

    h = 1469598103934665603ULL;
    for (size_t i = 0; i < k_sz; ++i) {
      h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(heap);
  return h;
}