#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <cstdint>

namespace {

// Comparator for deterministic tie-breaking: 
// Higher score first, then lower index first.
struct Better {
    bool operator()(const std::pair<int32_t, int>& a, const std::pair<int32_t, int>& b) const {
        if (a.first != b.first) return a.first > b.first;
        return a.second < b.second;
    }
};

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    if (beams <= 0 || vocab <= 0 || topk <= 0) return 0;
    
    // Pre-allocate buffer to avoid repeated allocations
    std::vector<std::pair<int32_t, int>> row;
    row.reserve(static_cast<std::size_t>(vocab));
    
    uint64_t hash = 0;
    Better comp;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            row.clear();
            const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
            
            // Fill the row with (score, index) pairs
            for (int v = 0; v < vocab; ++v) {
                row.push_back({logits[base + v], v});
            }

            // Use nth_element to find the top-k elements in O(N) average time.
            // This is significantly faster than a full O(N log N) sort.
            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end(), comp);
            }
            
            // nth_element only guarantees that the top-k elements are in the first k positions,
            // but they are not necessarily sorted among themselves. 
            // We must sort the first k elements to ensure deterministic checksum order.
            std::sort(row.begin(), row.begin() + topk, comp);

            // Update hash with the top-k results
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].first)));
                hash = mix(hash, static_cast<uint64_t>(row[k].second));
            }
        }
    }
    return hash;
}