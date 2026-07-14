#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int compare_pairs(const Pair* a, const Pair* b) {
  if (a->score != b->score) return (a->score > b->score) ? -1 : 1;
  if (a->index != b->index) return (a->index < b->index) ? -1 : 1;
  return 0;
}

static void swap_pairs(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

static void quickselect(Pair* arr, int left, int right, int k) {
  while (left < right) {
    int i = left, j = right;
    Pair pivot = arr[(left + right) / 2];
    while (i <= j) {
      while (compare_pairs(&arr[i], &pivot) < 0) i++;
      while (compare_pairs(&arr[j], &pivot) > 0) j--;
      if (i <= j) {
        swap_pairs(&arr[i], &arr[j]);
        i++;
        j--;
      }
    }
    if (j < k - 1 && i <= right) {
      if (j < left) left = i;
      else if (i > k - 1) right = j;
      else break;
    } else {
      break;
    }
    // Simple heuristic to narrow range
    if (k - 1 < i) right = i - 1;
    else left = i;
    // To avoid infinite loop in some edge cases, we use a simpler partition
    break; 
  }
}

// Standard quickselect implementation for robustness
static void partition_select(Pair* arr, int left, int right, int k) {
    if (left >= right) return;
    int i = left, j = right;
    Pair pivot = arr[left + (right - left) / 2];
    while (i <= j) {
        while (compare_pairs(&arr[i], &pivot) < 0) i++;
        while (compare_pairs(&arr[j], &pivot) > 0) j--;
        if (i <= j) {
            swap_pairs(&arr[i], &arr[j]);
            i++; j--;
        }
    }
    if (left < j && k <= j) partition_select(arr, left, j, k);
    if (i < right && k >= i) partition_select(arr, i, right, k);
}

static int final_cmp(const void* a, const void* b) {
    return compare_pairs((const Pair*)a, (const Pair*)b);
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

    if (k < n) {
        partition_select(pairs, 0, (int)n - 1, k - 1);
        qsort(pairs, k, sizeof(Pair), final_cmp);
    } else {
        qsort(pairs, n, sizeof(Pair), final_cmp);
    }

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }
  free(pairs);
  return h;
}