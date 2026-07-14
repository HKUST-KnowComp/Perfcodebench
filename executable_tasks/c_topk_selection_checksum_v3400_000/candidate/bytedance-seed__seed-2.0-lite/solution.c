#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int is_less(const Pair* a, const Pair* b) {
  if (a->score != b->score) {
    return a->score < b->score;
  }
  return a->index > b->index;
}

static inline void swap(Pair* a, Pair* b) {
  Pair temp = *a;
  *a = *b;
  *b = temp;
}

static void sift_up(Pair* heap, int pos) {
  while (pos > 0) {
    int parent = (pos - 1) / 2;
    if (is_less(&heap[pos], &heap[parent])) {
      swap(&heap[pos], &heap[parent]);
      pos = parent;
    } else {
      break;
    }
  }
}

static void sift_down(Pair* heap, int size, int pos) {
  while (1) {
    int left = 2 * pos + 1;
    int right = 2 * pos + 2;
    int smallest = pos;
    if (left < size && is_less(&heap[left], &heap[smallest])) {
      smallest = left;
    }
    if (right < size && is_less(&heap[right], &heap[smallest])) {
      smallest = right;
    }
    if (smallest != pos) {
      swap(&heap[pos], &heap[smallest]);
      pos = smallest;
    } else {
      break;
    }
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0 || iters <= 0) return 0;
  if ((size_t)k > n) k = (int)n;
  
  Pair* heap = (Pair*)malloc(k * sizeof(Pair));
  Pair* topk = (Pair*)malloc(k * sizeof(Pair));
  if (!heap || !topk) {
    free(heap);
    free(topk);
    return 0;
  }

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    int heap_size = 0;
    for (size_t i = 0; i < n; ++i) {
      Pair current = {scores[i], (uint32_t)i};
      if (heap_size < k) {
        heap[heap_size] = current;
        sift_up(heap, heap_size);
        heap_size++;
      } else {
        if (is_less(&heap[0], &current)) {
          heap[0] = current;
          sift_down(heap, heap_size, 0);
        }
      }
    }

    int temp_size = heap_size;
    for (int i = 0; i < k; ++i) {
      topk[i] = heap[0];
      heap[0] = heap[temp_size - 1];
      temp_size--;
      if (temp_size > 0) {
        sift_down(heap, temp_size, 0);
      }
    }

    for (int i = 0; i < k / 2; ++i) {
      swap(&topk[i], &topk[k - 1 - i]);
    }

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)topk[i].score << 32) ^ topk[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(heap);
  free(topk);
  return h;
}