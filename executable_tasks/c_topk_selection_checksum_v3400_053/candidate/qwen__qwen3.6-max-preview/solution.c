#include <stdint.h>
#include <stdlib.h>

static inline void swap64(uint64_t* a, uint64_t* b) {
    uint64_t t = *a; *a = *b; *b = t;
}

static void quickselect_topk(uint64_t* arr, size_t n, int k) {
    size_t left = 0, right = n - 1;
    size_t target = (size_t)k - 1;
    while (left < right) {
        size_t mid = left + (right - left) / 2;
        if (arr[left] < arr[mid]) swap64(&arr[left], &arr[mid]);
        if (arr[left] < arr[right]) swap64(&arr[left], &arr[right]);
        if (arr[mid] < arr[right]) swap64(&arr[mid], &arr[right]);
        
        uint64_t pivot = arr[mid];
        swap64(&arr[mid], &arr[right]);
        
        size_t store = left;
        for (size_t i = left; i < right; ++i) {
            if (arr[i] > pivot) {
                swap64(&arr[store], &arr[i]);
                store++;
            }
        }
        swap64(&arr[store], &arr[right]);
        
        if (store == target) break;
        if (store < target) {
            left = store + 1;
        } else {
            right = store - 1;
        }
    }
}

static void heap_sort_desc(uint64_t* arr, int k) {
    for (int i = k / 2 - 1; i >= 0; i--) {
        int root = i;
        while (1) {
            int child = 2 * root + 1;
            if (child >= k) break;
            if (child + 1 < k && arr[child] > arr[child + 1]) child++;
            if (arr[root] <= arr[child]) break;
            swap64(&arr[root], &arr[child]);
            root = child;
        }
    }
    for (int i = k - 1; i > 0; i--) {
        swap64(&arr[0], &arr[i]);
        int root = 0;
        while (1) {
            int child = 2 * root + 1;
            if (child >= i) break;
            if (child + 1 < i && arr[child] > arr[child + 1]) child++;
            if (arr[root] <= arr[child]) break;
            swap64(&arr[root], &arr[child]);
            root = child;
        }
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0) {
        uint64_t h = 0;
        for (int iter = 0; iter < iters; ++iter) h = 1469598103934665603ULL;
        return h;
    }
    if (n == 0) return 0;
    if ((size_t)k > n) k = (int)n;
    
    uint64_t* packed = (uint64_t*)malloc(n * sizeof(uint64_t));
    if (!packed) return 0;
    
    uint64_t h = 0;
    for (int iter = 0; iter < iters; ++iter) {
        for (size_t i = 0; i < n; ++i) {
            packed[i] = ((uint64_t)scores[i] << 32) | (UINT32_MAX - (uint32_t)i);
        }
        
        quickselect_topk(packed, n, k);
        heap_sort_desc(packed, k);
        
        h = 1469598103934665603ULL;
        for (int i = 0; i < k; ++i) {
            uint32_t idx = UINT32_MAX - (uint32_t)(packed[i] & 0xFFFFFFFFU);
            h ^= (packed[i] & 0xFFFFFFFF00000000ULL) ^ idx;
            h *= 1099511628211ULL;
        }
    }
    free(packed);
    return h;
}