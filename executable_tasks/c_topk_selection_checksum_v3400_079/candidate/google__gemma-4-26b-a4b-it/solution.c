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

static void swap_pairs(Pair* a, Pair* b) {
  Pair tmp = *a;
  *a = *b;
  *b = tmp;
}

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
        swap_pairs(&arr[i], &arr[j]);
        i++;
        j--;
      }
    }
    if (k <= j) right = j;
    else if (i <= k) left = i;
    else break;
  }
}

// Standard quicksort for the final k elements to ensure exact order
static void sort_k(Pair* arr, size_t k) {
  if (k <= 1) return;
  Pair pivot = arr[k / 2];
  size_t i = 0, j = k - 1;
  while (i <= j) {
    while (compare_pairs(arr[i], pivot) < 0) i++;
    while (compare_pairs(arr[j], pivot) > 0) j--;
    if (i <= j) {
      swap_pairs(&arr[i], &arr[j]);
      i++;
      j--;
    }
  }
  if (j > 0) sort_k(arr, j + 1);
  if (i < k) sort_k(arr + i, k - i);
}

// Using a simpler insertion sort for the small k to avoid recursion overhead
static void insertion_sort_k(Pair* arr, size_t k) {
  for (size_t i = 1; i < k; i++) {
    Pair key = arr[i];
    int j = (int)i - 1;
    while (j >= 0 && compare_pairs(arr[j], key) > 0) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

// Re-implementing qsort-like logic for the k elements to match original requirements
static int cmp_final(const void* a, const void* b) {
  const Pair* p1 = (const Pair*)a;
  const Pair* p2 = (const Pair*)b;
  if (p1->score != p2->score) return (p2->score > p1->score) ? 1 : -1;
  return (p1->index > p2->index) ? 1 : -1;
}

// Corrected comparison to match original logic exactly
static int cmp_pair_desc_final(const void* lhs, const void* rhs) {
  const Pair* a = (const Pair*) lhs;
  const Pair* b = (const Pair*) rhs;
  if (a->score < b->score) return 1;
  if (a->score > b->score) return -1;
  if (a->index < b->index) return -1;
  if (a->index > b->index) return 1;
  return 0;
}

#include <algorithm>

// Since we need to be strictly C and match the exact behavior:
// We use quickselect to get the top k, then sort only those k.

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
  if (n == 0 || k <= 0) return 0;
  Pair* pairs = (Pair*)malloc(n * sizeof(Pair));
  if (!pairs) return 0;

  uint64_t h = 0;
  for (int iter = 0; iter < iters; ++iter) {
    for (size_t i = 0; i < n; ++i) {
      pairs[i].score = scores[i];
      pairs[i].index = (uint32_t)i;
    }

    // Partial sort: move top k to the front
    // We use a standard quickselect approach
    size_t left = 0;
    size_t right = n - 1;
    size_t target = (size_t)k;

    while (left < right) {
      Pair pivot = pairs[left + (right - left) / 2];
      size_t i = left, j = right;
      while (i <= j) {
        while (cmp_pair_desc_final(&pairs[i], &pivot) < 0) i++;
        while (cmp_pair_desc_final(&pairs[j], &pivot) > 0) j--;
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
    // qsort is fine here because k is typically small
    qsort(pairs, k, sizeof(Pair), cmp_pair_desc_final);

    h = 1469598103934665603ULL;
    for (int i = 0; i < k; ++i) {
      h ^= ((uint64_t)pairs[i].score << 32) ^ pairs[i].index;
      h *= 1099511628211ULL;
    }
  }

  free(pairs);
  return h;
}