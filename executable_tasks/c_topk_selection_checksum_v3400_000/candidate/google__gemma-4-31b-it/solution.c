#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const Pair* a, const Pair* b) {
  if (a->score != b->score) return (a->score < b->score) ? 1 : -1;
  return (a->index < b->index) ? -1 : 1;
}

static void swap_pairs(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static void quickselect(Pair* pairs, int left, int right, int k) {
  if (left >= right) return;
  
  int i = left, j = right;
  Pair pivot = pairs[(left + right) / 2];
  
  while (i <= j) {
    while (compare_pairs(&pairs[i], &pivot) < 0) i++;
    while (compare_pairs(&pairs[j], &pivot) > 0) j--;
    if (i <= j) {
      swap_pairs(&pairs[i], &pairs[j]);
      i++;
      j--;
    }
  }
  
  if (left < j && j < k) quickselect(pairs, left, j, k);
  if (i < right && i > k) quickselect(pairs, i, right, k);
}

static int final_cmp(const void* a, const void* b) {
  return compare_pairs((const Pair*)a, (const Pair*)b);
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k == 0) return 0;
  
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }

    if ((int)n > k) {
      // Partition to get top k elements in the first k positions
      // Using a simple quickselect-like approach to isolate top k
      int left = 0, right = (int)n - 1;
      while (right > k - 1) {
        int i = left, j = right;
        Pair pivot = pairs[(left + right) / 2];
        while (i <= j) {
          while (compare_pairs(&pairs[i], &pivot) < 0) i++;
          while (compare_pairs(&pairs[j], &pivot) > 0) j--;
          if (i <= j) {
            swap_pairs(&pairs[i], &pairs[j]);
            i++; j--;
          }
        }
        if (j < k - 1) {
          left = i;
          if (left >= right) break;
        } else {
          right = j;
        }
      }
    }

    // Sort only the top k elements to ensure correct checksum order
    qsort(pairs, k, sizeof(Pair), final_cmp);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}