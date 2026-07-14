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

static void swap(Pair* a, Pair* b) {
  Pair temp = *a;
  *a = *b;
  *b = temp;
}

static int partition(Pair* arr, int left, int right) {
  Pair pivot = arr[right];
  int i = left;
  for (int j = left; j < right; j++) {
    // We want descending order for top-k, so 'smaller' means higher score
    if (compare_pairs(arr[j], pivot) < 0) {
      swap(&arr[i], &arr[j]);
      i++;
    }
  }
  swap(&arr[i], &arr[right]);
  return i;
}

static void quickselect(Pair* arr, int n, int k) {
  int left = 0, right = n - 1;
  while (left < right) {
    int pivotIndex = left + (right - left) / 2;
    // Simple median-of-three or just middle to avoid worst case on sorted data
    Pair midVal = arr[pivotIndex];
    if (compare_pairs(midVal, arr[left]) < 0) swap(&arr[left], &arr[pivotIndex]);
    if (compare_pairs(midVal, arr[right]) < 0) swap(&arr[right], &arr[pivotIndex]);
    // After swap, pivot is at pivotIndex, but let's just use standard partition
    
    // Using a more robust partition for the sake of the implementation
    int p = partition(arr, left, right);
    if (p == k) return;
    if (p < k) left = p + 1;
    else right = p - 1;
  }
}

// Since we need the top k to be EXACTLY ordered for the checksum,
// we use quickselect to find the k-th element, then sort only the first k elements.
// This is O(N + k log k).

static int qsort_cmp(const void* a, const void* b) {
  const Pair* pa = (const Pair*)a;
  const Pair* pb = (const Pair*)b;
  if (pa->score != pb->score) return (pb->score > pa->score) ? 1 : -1;
  return (pa->index > pb->index) ? 1 : -1;
}

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

    // Partial sort: Quickselect to get top k elements in the first k positions
    // Note: quickselect doesn't guarantee order within the partition, 
    // but it guarantees elements [0...k-1] are the top k.
    int left = 0, right = (int)n - 1;
    int target = k - 1;
    while (left < right) {
      int pivotIdx = left + (right - left) / 2;
      Pair pivot = pairs[pivotIdx];
      // Partition around pivot
      int i = left, j = right;
      while (i <= j) {
        while (compare_pairs(pairs[i], pivot) < 0) i++;
        while (compare_pairs(pairs[j], pivot) > 0) j--;
        if (i <= j) {
          swap(&pairs[i], &pairs[j]);
          i++;
          j--;
        }
      }
      if (target <= j) right = j;
      else if (target >= i) left = i;
      else break;
    }

    // The top k elements are now in pairs[0...k-1], but they are not necessarily sorted.
    // The checksum requires them to be ordered.
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
