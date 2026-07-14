#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const Pair a, const Pair b) {
  if (a.score != b.score) {
    return b.score - a.score; // Descending score
  }
  return (int)a.index - (int)b.index; // Ascending index
}

// Quickselect implementation to find the top-k elements
static void quickselect(Pair* arr, int n, int k) {
  int left = 0, right = n - 1;
  while (left < right) {
    Pair pivot = arr[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
      while (compare_pairs(arr[i], pivot) < 0) i++;
      while (compare_pairs(arr[j], pivot) > 0) j--;
      if (i <= j) {
        Pair tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
        i++;
        j--;
      }
    }
    if (k <= j) right = j;
    else if (i <= k) left = i;
    else break;
  }
}

// Standard qsort wrapper for the final k elements
static int qsort_cmp(const void* a, const void* b) {
  const Pair* pa = (const Pair*)a;
  const Pair* pb = (const Pair*)b;
  if (pa->score < pb->score) return 1;
  if (pa->score > pb->score) return -1;
  if (pa->index < pb->index) return -1;
  if (pa->index > pb->index) return 1;
  return 0;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  if (k > (int)n) k = (int)n;

  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  // Pre-fill indices and scores to avoid redundant work inside the loop if scores don't change
  // However, the task implies scores might be constant across iters, but we must follow the logic.
  // The baseline re-assigns scores[i] every iteration. 
  // If scores are constant, we only do the work once.
  
  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }

    // Use quickselect to partition the top k elements to the front
    // We want the 'smallest' k elements according to compare_pairs
    int left = 0, right = (int)n - 1;
    int target = k - 1;
    while (left < right) {
        Pair pivot = pairs[left + (right - left) / 2];
        int i = left, j = right;
        while (i <= j) {
            while (compare_pairs(pairs[i], pivot) < 0) i++;
            while (compare_pairs(pairs[j], pivot) > 0) j--;
            if (i <= j) {
                Pair tmp = pairs[i];
                pairs[i] = pairs[j];
                pairs[j] = tmp;
                i++;
                j--;
            }
        }
        if (target <= j) right = j;
        else if (i <= target) left = i;
        else break;
    }

    // Sort only the top k elements to ensure correct checksum order
    qsort(pairs, k, sizeof(Pair), qsort_cmp);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}