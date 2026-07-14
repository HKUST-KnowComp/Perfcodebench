#include "interface.h"

#include <cstdint>
#include <utility>
#include <vector>

namespace {

struct Entry {
    int32_t logit;
    int      idx;
};

inline bool better(const Entry& a, const Entry& b) {
    return (a.logit > b.logit) || (a.logit == b.logit && a.idx < b.idx);
}

inline void insert(Entry* heap, int k, Entry val) {
    if (better(val, heap[0])) {
        heap[0] = val;
        int i = 0;
        while (true) {
            int l = (i << 1) + 1;
            int r = l + 1;
            int worst = i;
            if (l < k && better(heap[worst], heap[l])) worst = l;
            if (r < k && better(heap[worst], heap[r])) worst = r;
            if (worst == i) break;
            std::swap(heap[i], heap[worst]);
            i = worst;
        }
    }
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams, int vocab, int topk, int iters) {
    std::vector<Entry> heap_storage(static_cast<size_t>(topk));
    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* row = &logits[static_cast<size_t>(b) * static_cast<size_t>(vocab)];

            // initialise min-heap with first k elements
            for (int i = 0; i < topk; ++i) {
                heap_storage[i] = Entry{row[i], i};
            }
            for (int i = topk / 2 - 1; i >= 0; --i) {
                int root = i;
                while (true) {
                    int l = (root << 1) + 1;
                    int r = l + 1;
                    int worst = root;
                    if (l < topk && better(heap_storage[worst], heap_storage[l])) worst = l;
                    if (r < topk && better(heap_storage[worst], heap_storage[r])) worst = r;
                    if (worst == root) break;
                    std::swap(heap_storage[root], heap_storage[worst]);
                    root = worst;
                }
            }

            // process remaining elements
            for (int v = topk; v < vocab; ++v) {
                insert(heap_storage.data(), topk, Entry{row[v], v});
            }

            // extract sorted order
            for (int k = topk - 1; k >= 0; --k) {
                std::swap(heap_storage[0], heap_storage[k]);
                int i = 0;
                while (true) {
                    int l = (i << 1) + 1;
                    int r = l + 1;
                    int worst = i;
                    if (l < k && better(heap_storage[worst], heap_storage[l])) worst = l;
                    if (r < k && better(heap_storage[worst], heap_storage[r])) worst = r;
                    if (worst == i) break;
                    std::swap(heap_storage[i], heap_storage[worst]);
                    i = worst;
                }
            }

            // hash results
            for (int k = 0; k < topk; ++k) {
                const Entry& e = heap_storage[k];
                hash ^= static_cast<uint64_t>(static_cast<uint32_t>(e.logit));
                hash *= 1099511628211ULL;
                hash ^= static_cast<uint64_t>(e.idx);
                hash *= 1099511628211ULL;
            }
        }
    }
    return hash;
}