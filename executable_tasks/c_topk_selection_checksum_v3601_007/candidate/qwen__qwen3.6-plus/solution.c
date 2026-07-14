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

static inline int is_less(Pair a, Pair b) {
    if (a.score != b.score) return a.score < b.score;
    return a.index > b.index;
}

static inline void sift_down(Pair* heap, size_t i, size_t size) {
    while (1) {
        size_t smallest = i;
        size_t left = 2 * i + 1;
        size_t right = 2 * i + 2;
        if (left < size && is_less(heap[left], heap[smallest])) smallest = left;
        if (right < size && is_less(heap[right], heap[smallest])) smallest = right;
        if (smallest == i) break;
        Pair tmp = heap[i];
        heap[i] = heap[smallest];
        heap[smallest] = tmp;
        i = smallest;
    }
}

static inline void sift_up(Pair* heap, size_t i) {
    while (i > 0) {
        size_t parent = (i - 1) / 2;
        if (is_less(heap[i], heap[parent])) {
            Pair tmp = heap[i];
            heap[i] = heap[parent];
            heap[parent] = tmp;
            i = parent;
        } else {
            break;
        }
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0) return 0;
    if ((size_t)k > n) k = (int)n;
    
    Pair* heap = (Pair*)malloc(k * sizeof(Pair));
    if (!heap) return 0;
    
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        size_t heap_size = 0;
        for (size_t i = 0; i < n; ++i) {
            Pair p = {scores[i], (uint32_t)i};
            if (heap_size < (size_t)k) {
                heap[heap_size] = p;
                sift_up(heap, heap_size);
                heap_size++;
            } else {
                if (is_less(heap[0], p)) {
                    heap[0] = p;
                    sift_down(heap, 0, heap_size);
                }
            }
        }
        
        qsort(heap, heap_size, sizeof(Pair), cmp_pair_desc);
        
        h = 1469598103934665603ULL;
        for (size_t i = 0; i < heap_size; ++i) {
            h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
            h *= 1099511628211ULL;
        }
    }
    free(heap);
    return h;
}