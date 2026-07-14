#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int is_better(const Pair* a, const Pair* b) {
  if (a->score != b->score) return a->score > b->score;
  return a->index < b->index;
}

static void swap(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static void quickselect(Pair* arr, int n, int k) {
  int left = 0, right = n - 1;
  while (left < right) {
    Pair pivot = arr[left + (right - left) / 2];
    int i = left, j = right;
    while (i <= j) {
      while (is_better(&arr[i], &pivot)) i++;
      while (is_better(&pivot, &arr[j])) j--;
      if (i <= j) {
        swap(&arr[i], &arr[j]);
        i++;
        j--;
      }
    }
    if (k <= j) right = j;
    else if (i <= k) left = i;
    else break;
  }
}

// Since the checksum requires the top-k to be strictly ordered,
// we use quickselect to partition the top-k, then sort only those k elements.
static int cmp_pair_desc(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score != b->score) return (a->score < b->score) ? 1 : -1;
  return (a->index < b->index) ? -1 : 1;
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  for (size_t i = 0; i < n; ++i) {
    pairs[i].score = scores[i];
    pairs[i].index = (uint32_t)i;
  }

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // We need the top k elements. Quickselect partitions the array such that 
    // the first k elements are the largest (according to is_better).
    // However, quickselect doesn't guarantee order within the partition.
    
    // Standard quickselect implementation to find k-th element
    int l = 0, r = (int)n - 1;
    int target = k - 1;
    while (l < r) {
        Pair pivot = pairs[l + (r - l) / 2];
        int i = l, j = r;
        while (i <= j) {
            while (is_better(&pairs[i], &pivot)) i++;
            while (is_better(&pivot, &pairs[j])) j--;
            if (i <= j) {
                swap(&pairs[i], &pairs[j]);
                i++;
                j--;
            }
        }
        if (target <= j) r = j;
        else if (target >= i) l = i;
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