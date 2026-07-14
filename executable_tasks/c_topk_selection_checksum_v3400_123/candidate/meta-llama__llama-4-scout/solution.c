#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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

// Heapify function to maintain the heap property
void heapify(Pair* heap, int i, int k) {
  int smallest = i;
  int left = 2 * i + 1;
  int right = 2 * i + 2;
  if (left < k && (heap[left].score < heap[smallest].score || (heap[left].score == heap[smallest].score && heap[left].index > heap[smallest].index))) {
    smallest = left;
  }
  if (right < k && (heap[right].score < heap[smallest].score || (heap[right].score == heap[smallest].score && heap[right].index > heap[smallest].index))) {
    smallest = right;
  }
  if (smallest != i) {
    Pair temp = heap[i];
    heap[i] = heap[smallest];
    heap[smallest] = temp;
    heapify(heap, smallest, k);
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
    // Build a min heap of size k
    for (int i = k / 2 - 1; i >= 0; --i) {
      heapify(pairs, i, k);
    }
    // Extract the top-k elements
    for (size_t i = k; i < n; ++i) {
      if (pairs[0].score < scores[i] || (pairs[0].score == scores[i] && pairs[0].index > i)) {
        pairs[0].score = scores[i];
        pairs[0].index = (uint32_t)i;
        heapify(pairs, 0, k);
      }
    }
    h = 1469598103934665603ULL;
    // Calculate checksum for top-k winners
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}