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

static void heapify(Pair* heap, int i, int k) {
  int smallest = i;
  int left = 2 * i + 1;
  int right = 2 * i + 2;
  if (left < k && (heap[left].score > heap[smallest].score || (heap[left].score == heap[smallest].score && heap[left].index > heap[smallest].index))) {
    smallest = left;
  }
  if (right < k && (heap[right].score > heap[smallest].score || (heap[right].score == heap[smallest].score && heap[right].index > heap[smallest].index))) {
    smallest = right;
  }
  if (smallest != i) {
    swap(&heap[i], &heap[smallest]);
    heapify(heap, smallest, k);
  }
}

static void build_min_heap(Pair* heap, int k) {
  for (int i = k / 2 - 1; i >= 0; --i) {
    heapify(heap, i, k);
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  Pair* top_k = (Pair*)malloc(k * sizeof(Pair));
  if (!top_k) return 0;
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < k; ++i) {
      top_k[i].score = scores[i];
      top_k[i].index = (uint32_t)i;
    }
    build_min_heap(top_k, k);
    for (size_t i = k; i < n; ++i) {
      if (scores[i] > top_k[0].score || (scores[i] == top_k[0].score && i < top_k[0].index)) {
        top_k[0].score = scores[i];
        top_k[0].index = (uint32_t)i;
        heapify(top_k, 0, k);
      }
    }
    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)top_k[i].score << 32) ^ top_k[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(top_k);
  return h;
}