#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int cmp_desc(const Pair* a, const Pair* b) {
  if (a->score != b->score) return (a->score > b->score) ? -1 : 1;
  return (a->index < b->index) ? -1 : 1;
}

static void insertion_sort(Pair* arr, int n) {
  for (int i = 1; i < n; i++) {
    Pair key = arr[i];
    int j = i - 1;
    while (j >= 0 && cmp_desc(&key, &arr[j]) < 0) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

static void sort_topk(Pair* arr, int n) {
  if (n <= 16) {
    insertion_sort(arr, n);
    return;
  }
  int mid = n / 2;
  Pair pivot;
  if (cmp_desc(&arr[0], &arr[mid]) < 0) {
    if (cmp_desc(&arr[mid], &arr[n-1]) < 0) pivot = arr[mid];
    else if (cmp_desc(&arr[0], &arr[n-1]) < 0) pivot = arr[n-1];
    else pivot = arr[0];
  } else {
    if (cmp_desc(&arr[0], &arr[n-1]) < 0) pivot = arr[0];
    else if (cmp_desc(&arr[mid], &arr[n-1]) < 0) pivot = arr[n-1];
    else pivot = arr[mid];
  }
  int i = 0, j = n - 1;
  while (i <= j) {
    while (cmp_desc(&arr[i], &pivot) < 0) i++;
    while (cmp_desc(&arr[j], &pivot) > 0) j--;
    if (i <= j) {
      Pair tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
      i++; j--;
    }
  }
  if (j > 0) sort_topk(arr, j + 1);
  if (i < n - 1) sort_topk(arr + i, n - i);
}

static void quickselect_k(Pair* arr, int n, int k) {
  int lo = 0, hi = n - 1;
  while (lo < hi) {
    int mid = lo + (hi - lo) / 2;
    Pair pivot;
    if (cmp_desc(&arr[lo], &arr[mid]) < 0) {
      if (cmp_desc(&arr[mid], &arr[hi]) < 0) pivot = arr[mid];
      else if (cmp_desc(&arr[lo], &arr[hi]) < 0) pivot = arr[hi];
      else pivot = arr[lo];
    } else {
      if (cmp_desc(&arr[lo], &arr[hi]) < 0) pivot = arr[lo];
      else if (cmp_desc(&arr[mid], &arr[hi]) < 0) pivot = arr[hi];
      else pivot = arr[mid];
    }
    int i = lo, j = hi;
    while (i <= j) {
      while (cmp_desc(&arr[i], &pivot) < 0) i++;
      while (cmp_desc(&arr[j], &pivot) > 0) j--;
      if (i <= j) {
        Pair tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        i++; j--;
      }
    }
    if (k - 1 <= j) {
      hi = j;
    } else if (k - 1 >= i) {
      lo = i;
    } else {
      break;
    }
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
    if (k < (int)n) {
      quickselect_k(pairs, (int)n, k);
      sort_topk(pairs, k);
    } else {
      sort_topk(pairs, (int)n);
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