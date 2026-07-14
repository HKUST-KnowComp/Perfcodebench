#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct Element {
    int32_t logit;
    int idx;
};

inline bool better(const Element& a, const Element& b) {
    return a.logit > b.logit || (a.logit == b.logit && a.idx < b.idx);
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    if (iters <= 0) return 0;

    std::vector<Element> row(static_cast<size_t>(vocab));
    uint64_t hash = 1469598103934665603ULL;
    int k = std::min(topk, vocab);

    for (int b = 0; b < beams; ++b) {
        size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
        for (int v = 0; v < vocab; ++v) {
            row[v] = {logits[base + v], v};
        }

        if (k < vocab) {
            std::nth_element(row.begin(), row.begin() + k, row.end(), better);
            std::sort(row.begin(), row.begin() + k, better);
        } else {
            std::sort(row.begin(), row.end(), better);
        }

        for (int i = 0; i < k; ++i) {
            hash = mix(hash, static_cast<uint32_t>(row[i].logit));
            hash = mix(hash, static_cast<uint32_t>(row[i].idx));
        }
    }
    return hash;
}
