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

// Quickselect implementation to find the top-k elements in O(n) average time
static void quickselect(Pair* arr, size_t n, size_t k) {
  size_t left = 0;
  size_t right = n - 1;
  while (left < right) {
    Pair pivot = arr[(left + right) / 2];
    size_t i = left;
    size_t j = right;
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

// Standard quicksort for the final k elements to ensure correct checksum order
static int qsort_cmp(const void* a, const void* b) {
  const Pair* pa = (const Pair*)a;
  const Pair* pb = (const Pair*)b;
  if (pa->score != pb->score) return (pb->score > pa->score) ? 1 : -1;
  return (pa->index > pb->index) ? 1 : -1;
}

// Re-implementing the specific comparison logic required by the original qsort
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
  if (n == 0 || k <= 0) return 0;
  if ((size_t)k > n) k = (int)n;

  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  // Pre-fill indices to avoid repeated work inside the loop
  for (size_t i = 0; i < n; ++i) {
    pairs[i].index = (uint32_t)i;
  }

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
    }

    // Use quickselect to partition the top k elements to the front
    // We use a manual partition logic to match the exact sorting requirements
    // Since the requirement is the exact top-k ordered, we must sort the top k.
    
    // Using a simpler approach for the partition to ensure correctness with the specific tie-breaking rules
    // A full qsort on the whole array is O(N log N), but quickselect is O(N).
    // To handle the tie-breaking (index) correctly in quickselect, the comparison must be identical.
    
    size_t l = 0, r = n - 1;
    while (l < r) {
        Pair pivot = pairs[l + (r - l) / 2];
        size_t i = l, j = r;
        while (i <= j) {
            while (cmp_pair_desc(&pairs[i], &pivot) < 0) i++;
            while (cmp_pair_desc(&pairs[j], &pivot) > 0) j--;
            if (i <= j) {
                Pair tmp = pairs[i];
                pairs[i] = pairs[j];
                pairs[j] = tmp;
                i++;
                j--;
            }
        }
        if ((size_t)k <= j) r = j;
        else if (i <= (size_t)k) l = i;
        else break;
    }

    // Sort only the top k elements to satisfy the checksum requirement
    qsort(pairs, k, sizeof(Pair), cmp_pair_desc);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}