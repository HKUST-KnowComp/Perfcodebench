#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <cstdint>

namespace {

// Tie-breaking: higher score first, then lower index first.
// To use std::nth_element/std::sort, we define a strict weak ordering.
// We want the 'top' elements to be at the beginning.
struct Element {
    int32_t score;
    int32_t index;

    // Returns true if 'a' is better than 'b'
    bool operator>(const Element& other) const {
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
    
    // Ensure topk does not exceed vocab
    int actual_topk = std::min(topk, vocab);
    
    // Pre-allocate buffer to avoid repeated allocations
    std::vector<Element> row(static_cast<size_t>(vocab));
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
            
            // Load data into the buffer
            for (int v = 0; v < vocab; ++v) {
                row[v] = {logits[base + v], v};
            }

            // Use nth_element to find the top-k elements in O(V) average time
            std::nth_element(row.begin(), row.begin() + actual_topk, row.end(), std::greater<Element>());
            
            // The first 'actual_topk' elements are the top ones, but they are not necessarily sorted among themselves.
            // The checksum requires a specific order (implied by the original sort).
            // We must sort only the top-k elements to ensure deterministic checksum.
            std::sort(row.begin(), row.begin() + actual_topk, std::greater<Element>());

            for (int k = 0; k < actual_topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].index)));
            }
        }
    }
    return hash;
}
