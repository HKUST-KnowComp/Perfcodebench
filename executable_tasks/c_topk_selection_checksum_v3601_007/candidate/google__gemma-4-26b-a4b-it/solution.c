#include <stdint.h>
#include <stdlib.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

static inline int cmp_pair_desc(const Pair* a, const Pair* b) {
  if (a->score != b->score) return (a->score < b->score) ? 1 : -1;
  return (a->index > b->index) ? 1 : -1;
}

static inline int cmp_pair_asc(const Pair* a, const Pair* b) {
  if (a->score != b->score) return (a->score > b->score) ? 1 : -1;
  return (a->index > b->index) ? 1 : -1;
}

// Standard quickselect implementation to partition top-k elements
static void quickselect(Pair* arr, size_t n, size_t k) {
    size_t left = 0, right = n - 1;
    while (left < right) {
        Pair pivot = arr[(left + right) / 2];
        size_t i = left, j = right;
        while (i <= j) {
            while (cmp_pair_desc(&arr[i], &pivot) < 0) i++;
            while (cmp_pair_desc(&arr[j], &pivot) > 0) j--;
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

// Using a simpler partition for stability and correctness in top-k
static void partition_top_k(Pair* arr, size_t n, size_t k) {
    size_t left = 0, right = n - 1;
    while (left < right) {
        Pair pivot = arr[left + (right - left) / 2];
        size_t i = left, j = right;
        while (i <= j) {
            while (cmp_pair_desc(&arr[i], &pivot) < 0) i++;
            while (cmp_pair_desc(&arr[j], &pivot) > 0) j--;
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

// Comparison for qsort on the small k subset
static int qsort_cmp(const void* a, const void* b) {
    return cmp_pair_desc((const Pair*)a, (const Pair*)b);
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

        // Use quickselect to move top k elements to the front
        // We use a standard nth_element style approach
        size_t l = 0, r = n - 1;
        size_t target = (size_t)k - 1;
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
            if (target <= j) r = j;
            else if (target >= i) l = i;
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