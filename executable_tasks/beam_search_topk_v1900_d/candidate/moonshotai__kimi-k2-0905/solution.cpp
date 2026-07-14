#include "interface.h"
#include <algorithm>
#include <climits>
#include <cstdint>
#include <utility>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams,
                            int vocab,
                            int topk,
                            int iters) {
    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        for (int b = 0; b < beams; ++b) {
            const int32_t* ptr = &logits[static_cast<size_t>(b) * static_cast<size_t>(vocab)];

            struct Entry { int32_t score; int idx; };
            Entry top[16]; // support up to topk==16
            int len = 0;

            for (int v = 0; v < vocab; ++v) {
                int32_t s = ptr[v];
                if (len < topk) {
                    top[len].score = s;
                    top[len].idx   = v;
                    ++len;
                    if (len == topk) {
                        // build min-heap on first topk
                        std::make_heap(top, top + topk, [](const Entry& a, const Entry& b) {
                            return a.score > b.score || (a.score == b.score && a.idx < b.idx);
                        });
                    }
                } else {
                    // compare to heap root
                    if (s > top[0].score || (s == top[0].score && v < top[0].idx)) {
                        std::pop_heap(top, top + topk, [](const Entry& a, const Entry& b) {
                            return a.score > b.score || (a.score == b.score && a.idx < b.idx);
                        });
                        top[topk - 1].score = s;
                        top[topk - 1].idx   = v;
                        std::push_heap(top, top + topk, [](const Entry& a, const Entry& b) {
                            return a.score > b.score || (a.score == b.score && a.idx < b.idx);
                        });
                    }
                }
            }

            if (len == topk) {
                std::sort_heap(top, top + topk, [](const Entry& a, const Entry& b) {
                    return a.score > b.score || (a.score == b.score && a.idx < b.idx);
                });
            }

            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k].score)));
                hash = mix(hash, static_cast<uint64_t>(top[k].idx));
            }
        }
    }
    return hash;
}