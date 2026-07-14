#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  uint32_t score;
  uint32_t index;
} Pair;

/* Min-heap operations for Pair (by score desc, index asc means min-heap uses score asc, index desc) */
/* We want top-k highest scores. Use min-heap: root is smallest score in heap. */
/* If new score > root, replace root and sift down. */

static inline void heap_sift_down(Pair* heap, int heap_size, int i) {
    while (1) {
        int smallest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        
        /* Compare: smaller score is "smaller", or same score with larger index is "smaller" */
        if (left < heap_size) {
            if (heap[left].score < heap[smallest].score ||
                (heap[left].score == heap[smallest].score && heap[left].index > heap[smallest].index)) {
                smallest = left;
            }
        }
        if (right < heap_size) {
            if (heap[right].score < heap[smallest].score ||
                (heap[right].score == heap[smallest].score && heap[right].index > heap[smallest].index)) {
                smallest = right;
            }
        }
        
        if (smallest == i) break;
        
        Pair tmp = heap[i];
        heap[i] = heap[smallest];
        heap[smallest] = tmp;
        i = smallest;
    }
}

static inline void heap_sift_up(Pair* heap, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        /* If current is smaller than parent, swap */
        if (heap[i].score < heap[parent].score ||
            (heap[i].score == heap[parent].score && heap[i].index > heap[parent].index)) {
            Pair tmp = heap[i];
            heap[i] = heap[parent];
            heap[parent] = tmp;
            i = parent;
        } else {
            break;
        }
    }
}

/* Insertion sort for small arrays - sorts descending by score, ascending by index on tie */
static void insertion_sort_desc(Pair* arr, int n) {
    for (int i = 1; i < n; i++) {
        Pair key = arr[i];
        int j = i - 1;
        while (j >= 0 && (arr[j].score < key.score ||
               (arr[j].score == key.score && arr[j].index > key.index))) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

uint64_t run(const uint32_t* scores, size_t n, int k, int iters) {
    if (k <= 0 || n == 0) return 0;
    
    size_t kk = (size_t)k;
    if (kk > n) kk = n;
    
    Pair* heap = (Pair*)malloc(kk * sizeof(Pair));
    if (!heap) return 0;
    
    uint64_t h = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        /* Build initial heap with first k elements */
        for (size_t i = 0; i < kk; ++i) {
            heap[i].score = scores[i];
            heap[i].index = (uint32_t)i;
        }
        
        /* Heapify - build min-heap */
        for (int i = (int)(kk / 2) - 1; i >= 0; i--) {
            heap_sift_down(heap, (int)kk, i);
        }
        
        /* Process remaining elements */
        for (size_t i = kk; i < n; ++i) {
            uint32_t sc = scores[i];
            /* If this score is greater than heap min, or equal with smaller index, replace */
            if (sc > heap[0].score ||
                (sc == heap[0].score && (uint32_t)i < heap[0].index)) {
                heap[0].score = sc;
                heap[0].index = (uint32_t)i;
                heap_sift_down(heap, (int)kk, 0);
            }
        }
        
        /* Sort the k elements in descending order */
        insertion_sort_desc(heap, (int)kk);
        
        /* Compute checksum */
        h = 1469598103934665603ULL;
        for (size_t i = 0; i < kk; ++i) {
            h ^= ((uint64_t)heap[i].score << 32) ^ heap[i].index;
            h *= 1099511628211ULL;
        }
    }
    
    free(heap);
    return h;
}