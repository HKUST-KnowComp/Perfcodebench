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

static void sift_down(Pair* heap, size_t heap_size, size_t root) {
  while (1) {
    size_t left = 2 * root + 1;
    size_t smallest = root;
    if (left < heap_size && cmp_pair_desc(&heap[left], &heap[smallest]) < 0) {
      smallest = left;
    }
    size_t right = left + 1;
    if (right < heap_size && cmp_pair_desc(&heap[right], &heap[smallest]) < 0) {
      smallest = right;
    }
    if (smallest == root) break;
    Pair temp = heap[root];
    heap[root] = heap[smallest];
    heap[smallest] = temp;
    root = smallest;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;
  
  // Initialize pairs once since scores are immutable across iterations
  for (size_t i = 0; i < n; ++i) {
    pairs[i].score = scores[i];
    pairs[i].index = (uint32_t)i;
  }
  
  uint64_t h = 0;
  const size_t topk = (size_t)k < n ? (size_t)k : n;
  
  for (int iter = 0; iter < iters; ++iter) {
    Pair* top_pairs = pairs;
    size_t heap_size = 0;
    
    for (size_t i = 0; i < n; ++i) {
      const Pair curr = pairs[i];
      if (heap_size < topk) {
        top_pairs[heap_size] = curr;
        size_t child = heap_size;
        heap_size++;
        // Sift up
        while (child > 0) {
          size_t parent = (child - 1) / 2;
          if (cmp_pair_desc(&top_pairs[child], &top_pairs[parent]) < 0) {
            Pair temp = top_pairs[parent];
            top_pairs[parent] = top_pairs[child];
            top_pairs[child] = temp;
            child = parent;
          } else break;
        }
      } else if (cmp_pair_desc(&curr, &top_pairs[0]) > 0) {
        top_pairs[0] = curr;
        sift_down(top_pairs, heap_size, 0);
      }
    }
    
    // Sort top k to match original order, compute checksum
    qsort(top_pairs, topk, sizeof(Pair), cmp_pair_desc);
    h = 1469598103934665603ULL;
    for (size_t i = 0; i < topk; ++i) {
      h ^= ((uint64_t)top_pairs[i].score << 32) ^ top_pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  
  free(pairs);
  return h;
}