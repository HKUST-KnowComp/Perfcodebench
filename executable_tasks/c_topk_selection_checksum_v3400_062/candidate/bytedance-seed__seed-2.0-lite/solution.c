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

static void sift_down(Pair* pairs, size_t heap_size, size_t i) {
  while (1) {
    size_t left = 2 * i + 1;
    size_t right = 2 * i + 2;
    size_t worst = i;
    if (left < heap_size && cmp_pair_desc(&pairs[worst], &pairs[left]) > 0) {
      worst = left;
    }
    if (right < heap_size && cmp_pair_desc(&pairs[worst], &pairs[right]) > 0) {
      worst = right;
    }
    if (worst == i) break;
    Pair temp = pairs[i];
    pairs[i] = pairs[worst];
    pairs[worst] = temp;
    i = worst;
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
    size_t k_eff = (k > 0 && (size_t)k < n) ? (size_t)k : n;
    if (k_eff > 0) {
      // Build min-heap of top k candidates
      for (size_t i = k_eff / 2 - 1; i != (size_t)-1; i--) {
        sift_down(pairs, k_eff, i);
      }
      // Process remaining elements to filter top k
      for (size_t i = k_eff; i < n; i++) {
        if (cmp_pair_desc(&pairs[i], &pairs[0]) < 0) {
          pairs[0] = pairs[i];
          sift_down(pairs, k_eff, 0);
        }
      }
      // Sort top k to match original order for correct checksum
      qsort(pairs, k_eff, sizeof(Pair), cmp_pair_desc);
    }
    h = 1469598103934665603ULL;
    for (int i = 0; i < k && (size_t)i < n; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}