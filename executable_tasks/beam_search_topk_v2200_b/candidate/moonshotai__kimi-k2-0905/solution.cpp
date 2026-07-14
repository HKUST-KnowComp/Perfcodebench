#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

inline bool better(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) {
    return (a.first > b.first) || ((a.first == b.first) && (a.second < b.second));
}

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits,
                            int beams,
                            int vocab,
                            int topk,
                            int iters) {
    std::vector<std::pair<int32_t, int>> row;
    row.reserve(static_cast<size_t>(vocab));

    const size_t vocab_sz = static_cast<size_t>(vocab);
    const size_t topk_sz  = static_cast<size_t>(topk);

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        for (int b = 0; b < beams; ++b) {
            row.clear();
            const size_t base = static_cast<size_t>(b) * vocab_sz;
            for (size_t v = 0; v < vocab_sz; ++v) {
                row.emplace_back(logits[base + v], static_cast<int>(v));
            }
            // nth_element partitions so that the top-k are at the front (unsorted among themselves)
            std::nth_element(row.begin(), row.begin() + topk_sz, row.end(), better);
            // sort only those top-k to establish exact order for deterministic tie-breaking
            std::sort(row.begin(), row.begin() + topk_sz, better);

            // unroll mixing loop 4x to reduce loop overhead
            size_t k = 0;
            for (; k + 3 < topk_sz; k += 4) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].first)));
                hash = mix(hash, static_cast<uint64_t>(row[k].second));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k+1].first)));
                hash = mix(hash, static_cast<uint64_t>(row[k+1].second));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k+2].first)));
                hash = mix(hash, static_cast<uint64_t>(row[k+2].second));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k+3].first)));
                hash = mix(hash, static_cast<uint64_t>(row[k+3].second));
            }
            for (; k < topk_sz; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].first)));
                hash = mix(hash, static_cast<uint64_t>(row[k].second));
            }
        }
    }
    return hash;
}