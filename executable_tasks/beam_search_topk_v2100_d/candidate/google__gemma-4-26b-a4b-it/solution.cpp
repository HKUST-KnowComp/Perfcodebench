#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

// Tie-breaking: higher logit first, then lower index first.
// To use std::nth_element/std::sort, we define a strict weak ordering.
// We want the 'top' elements to be at the beginning.
struct Element {
    int32_t logit;
    int32_t index;

    // Returns true if 'a' is better than 'b'
    bool operator>(const Element& other) const {
        if (logit != other.logit) {
            return logit > other.logit;
        }
        return index < other.index;
    }
};

// Comparator for std::nth_element and std::sort to match the 'better' logic.
// We want the elements that satisfy 'better' to come first.
struct BetterComparator {
    bool operator()(const Element& a, const Element& b) const {
        if (a.logit != b.logit) {
            return a.logit > b.logit;
        }
        return a.index < b.index;
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
    const BetterComparator comp;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
            
            // Fill the buffer
            for (int v = 0; v < vocab; ++v) {
                row[v] = {logits[base + v], v};
            }

            // Use nth_element to find the top-k elements in O(V) average time
            // instead of O(V log V) full sort.
            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end(), comp);
                // The top-k elements are now in [begin, begin + topk), but not necessarily sorted.
                // We must sort them to ensure deterministic checksum calculation.
                std::sort(row.begin(), row.begin() + topk, comp);
            } else {
                std::sort(row.begin(), row.end(), comp);
            }

            // Calculate hash for the top-k elements
            int actual_topk = std::min(topk, vocab);
            for (int k = 0; k < actual_topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].logit)));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].index)));
            }
        }
    }
    return hash;
}
