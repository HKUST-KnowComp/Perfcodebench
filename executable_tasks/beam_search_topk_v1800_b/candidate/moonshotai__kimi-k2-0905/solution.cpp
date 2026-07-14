#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

inline bool better(int32_t va, int ia, int32_t vb, int ib) {
    return (va > vb) || (va == vb && ia < ib);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams, int vocab, int topk, int iters) {
    std::vector<std::pair<int32_t, int>> top(topk);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int base = b * vocab;

            // --- fill with first topk items ---
            for (int i = 0; i < topk; ++i) {
                top[i].first  = logits[base + i];
                top[i].second = i;
            }

            // --- deterministic min-heap of size <= topk ---
            // heap property: top[0] is the smallest among the k kept so far
            for (int i = 1; i < topk; ++i) {
                int pos = i;
                while (pos > 0) {
                    int parent = (pos - 1) >> 1;
                    if (better(top[parent].first,  top[parent].second,
                               top[pos].first,     top[pos].second)) break;
                    std::swap(top[parent], top[pos]);
                    pos = parent;
                }
            }

            // --- scan remaining vocabulary ---
            for (int v = topk; v < vocab; ++v) {
                int32_t val = logits[base + v];
                if (better(top[0].first,  top[0].second,
                        val,           v)) continue;

                // replace root
                top[0].first  = val;
                top[0].second = v;

                // sift-down
                int pos = 0;
                for (;;) {
                    int left  = (pos << 1) + 1;
                    if (left >= topk) break;
                    int right = left + 1;
                    int smallest = left;
                    if (right < topk &&
                        better(top[right].first, top[right].second,
                               top[left].first,   top[left].second))
                        smallest = right;
                    if (better(top[pos].first, top[pos].second,
                               top[smallest].first, top[smallest].second)) break;
                    std::swap(top[pos], top[smallest]);
                    pos = smallest;
                }
            }

            // --- deterministic sort of the k selected items ---
            // insertion sort is fastest for tiny k (<=20 in practice)
            for (int i = 1; i < topk; ++i) {
                auto key = top[i];
                int j = i - 1;
                while (j >= 0 && better(key.first, key.second,
                                        top[j].first, top[j].second)) {
                    top[j + 1] = top[j];
                    --j;
                }
                top[j + 1] = key;
            }

            // --- accumulate checksum ---
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k].first)));
                hash = mix(hash, static_cast<uint64_t>(top[k].second));
            }
        }
    }
    return hash;
}