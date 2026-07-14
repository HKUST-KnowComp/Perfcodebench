#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static int better(Pair a, Pair b) {
  if (a.score != b.score) return a.score > b.score;
  return a.index < b.index;
}

static int worse(Pair a, Pair b) {
  if (a.score != b.score) return a.score < b.score;
  return a.index > b.index;
}

static void heap_swap(Pair* a, Pair* b) {
  Pair t = *a; *a = *b; *b = t;
}

static void sift_up(Pair* heap, int pos) {
  while (pos > 0) {
    int parent = (pos - 1) / 2;
    if (!worse(heap[pos], heap[parent])) break;
    heap_swap(&heap[pos], &heap[parent]);
    pos = parent;
  }
}

static void sift_down(Pair* heap, int size, int pos) {
  for (;;) {
    int left = pos * 2 + 1;
    int right = left + 1;
    int smallest = pos;
    if (left < size && worse(heap[left], heap[smallest])) smallest = left;
    if (right < size && worse(heap[right], heap[smallest])) smallest = right;
    if (smallest == pos) break;
    heap_swap(&heap[pos], &heap[smallest]);
    pos = smallest;
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
  Pair* heap = (Pair*)malloc((size_t)k * sizeof(Pair));
  Pair* out = (Pair*)malloc((size_t)k * sizeof(Pair));
  if (!heap || !out) return 0;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int size = 0;
    for (size_t i = 0; i < n; ++i) {
      Pair cur = {scores[i], (uint32_t)i};
      if (size < k) {
        heap[size] = cur;
        sift_up(heap, size);
        ++size;
      } else if (better(cur, heap[0])) {
        heap[0] = cur;
        sift_down(heap, size, 0);
      }
    }
    for (int i = 0; i < k; ++i) out[i] = heap[i];
    qsort(out, (size_t)k, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)out[i].score << 32) ^ out[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(heap);
  free(out);
  return h;
}
