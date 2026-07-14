#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int cmp_pair_desc(const Pair* a, const Pair* b) {
  if (a->score != b->score) return (b->score < a->score) ? -1 : 1;
  if (a->index != b->index) return (a->index < b->index) ? -1 : 1;
  return 0;
}

static void swap_pairs(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static int partition(Pair* pairs, int left, int right) {
  Pair pivot = pairs[right];
  int i = left;
  for (int j = left; j < right; j++) {
    if (cmp_pair_desc(&pairs[j], &pivot) < 0) {
      swap_pairs(&pairs[i], &pairs[j]);
      i++;
    }
  }
  swap_pairs(&pairs[i], &pairs[right]);
  return i;
}

static void quickselect(Pair* pairs, int left, int right, int k) {
  while (left < right) {
    int pivot_idx = partition(pairs, left, right);
    if (pivot_idx == k) return;
    if (pivot_idx < k) left = pivot_idx + 1;
    else right = pivot_idx - 1;
  }
}

static void insertion_sort(Pair* pairs, int k) {
  for (int i = 1; i < k; i++) {
    Pair key = pairs[i];
    int j = i - 1;
    while (j >= 0 && cmp_pair_desc(&pairs[j], &key) > 0) {
      pairs[j + 1] = pairs[j];
      j--;
    }
    pairs[j + 1] = key;
  }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  // Pre-fill indices to avoid re-assignment in loop if scores don't change
  // However, since scores are passed per run, we must update scores.
  // But we can optimize the loop structure.
  
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }

    if (k < (int)n) {
      quickselect(pairs, 0, (int)n - 1, k);
    }
    
    // The top-k must be sorted for the checksum
    insertion_sort(pairs, k);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}