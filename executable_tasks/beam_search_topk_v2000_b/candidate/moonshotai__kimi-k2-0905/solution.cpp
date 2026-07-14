#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

// In-place nth_element that partitions [0, n) so the largest elements
// (according to the deterministic rule) are at the front.
// Returns pointer one past the k-th element.
inline int* nth_element_int(int* first, int* last, int k, const int32_t* scores) {
    while (true) {
        int* mid = first + ((last - first) >> 1);
        int pivot = *mid;
        int* i = first;
        int* j = last - 1;
        while (i <= j) {
            while (scores[*i] > scores[pivot] ||
                   (scores[*i] == scores[pivot] && *i < pivot)) ++i;
            while (scores[*j] < scores[pivot] ||
                   (scores[*j] == scores[pivot] && *j > pivot)) --j;
            if (i <= j) {
                std::swap(*i, *j);
                ++i; --j;
            }
        }
        if (j - first + 1 >= k) {
            last = j + 1;
        } else {
            k -= (i - first);
            first = i;
        }
        if (first >= last) return first;
    }
}

} // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams,
                            int vocab,
                            int topk,
                            int iters) {
    const std::size_t vocab_sz = static_cast<std::size_t>(vocab);
    std::vector<int> indices(vocab);
    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* const scores = &logits[b * vocab];
            for (int v = 0; v < vocab; ++v) indices[v] = v;

            int* end = nth_element_int(indices.data(), indices.data() + vocab, topk, scores);
            std::sort(indices.data(), end,
                      [&](int a, int b) {
                          return scores[a] > scores[b] || (scores[a] == scores[b] && a < b);
                      });

            for (int k = 0; k < topk; ++k) {
                const int v = indices[k];
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(scores[v])));
                hash = mix(hash, static_cast<uint64_t>(v));
            }
        }
    }
    return hash;
}
