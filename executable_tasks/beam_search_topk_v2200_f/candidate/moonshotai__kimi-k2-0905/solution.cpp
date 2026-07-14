#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

inline bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
    return a.first > b.first || (a.first == b.first && a.second < b.second);
}

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams,
                            int vocab,
                            int topk,
                            int iters) {
    std::vector<std::pair<int32_t, int>> row;
    row.reserve(static_cast<size_t>(vocab));

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            row.clear();
            const int32_t* __restrict ptr = &logits[static_cast<size_t>(b) * static_cast<size_t>(vocab)];
            for (int v = 0; v < vocab; ++v) {
                row.emplace_back(ptr[v], v);
            }
            if (static_cast<size_t>(topk) < row.size()) {
                std::nth_element(row.begin(), row.begin() + topk, row.end(), better);
                std::sort(row.begin(), row.begin() + topk, better);
            } else {
                std::sort(row.begin(), row.end(), better);
            }
            const auto* top = row.data();
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top[k].first)));
                hash = mix(hash, static_cast<uint64_t>(top[k].second));
            }
        }
    }
    return hash;
}