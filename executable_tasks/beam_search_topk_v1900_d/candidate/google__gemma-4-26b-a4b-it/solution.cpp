#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

// Tie-breaking: higher score first, then lower index.
// To use std::nth_element/std::sort, we define a strict weak ordering.
// We want the 'top' elements to be at the beginning.
struct Element {
    int32_t score;
    int index;

    // Returns true if 'a' is better than 'b'
    bool operator<(const Element& other) const {
        if (score != other.score) {
            return score > other.score;
        }
        return index < other.index;
    }
};

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    if (vocab <= 0 || beams <= 0 || topk <= 0) return 0;
    
    // Pre-allocate buffer to avoid repeated allocations
    std::vector<Element> row(static_cast<size_t>(vocab));
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
            
            // Load logits into the Element buffer
            for (int v = 0; v < vocab; ++v) {
                row[v] = {logits[base + v], v};
            }

            // Use nth_element to find the top-k elements in O(V) average time
            // The top-k elements will be in the range [0, topk)
            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end());
                // nth_element doesn't guarantee order within the top-k, 
                // so we must sort the top-k part to ensure deterministic checksum
                std::sort(row.begin(), row.begin() + topk);
            } else {
                std::sort(row.begin(), row.end());
            }

            // Accumulate hash
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
                hash = mix(hash, static_cast<uint64_t>(row[k].index));
            }
        }
    }
    return hash;
}
