#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct Entry {
    int32_t score;
    int idx;
};

inline bool better(const Entry& a, const Entry& b) noexcept {
    return a.score > b.score || (a.score == b.score && a.idx < b.idx);
}

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    std::vector<Entry> row(static_cast<std::size_t>(vocab));
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* base = logits.data() + static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
            
            // Fill the row buffer
            for (int v = 0; v < vocab; ++v) {
                row[static_cast<std::size_t>(v)] = {base[v], v};
            }
            
            // Use nth_element to partition around the k-th element
            // This is O(n) on average instead of O(n log n) for full sort
            auto topk_end = row.begin() + topk;
            std::nth_element(row.begin(), topk_end - 1, row.end(), better);
            
            // Now sort just the top-k elements for deterministic ordering
            std::sort(row.begin(), topk_end, better);
            
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[static_cast<std::size_t>(k)].score)));
                hash = mix(hash, static_cast<uint64_t>(row[static_cast<std::size_t>(k)].idx));
            }
        }
    }
    return hash;
}