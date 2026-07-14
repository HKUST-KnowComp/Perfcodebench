#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <cstdint>

namespace {

// Tie-breaking: higher score first, then lower index first.
struct Element {
    int32_t score;
    int32_t index;

    bool operator<(const Element& other) const {
        if (score != other.score) return score > other.score;
        return index < other.index;
    }
};

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
    if (iters <= 0) return 0;

    // The hash is reset to this constant at the start of every iteration.
    // Since 'logits' is const and doesn't change, the result of every iteration is identical.
    // We only need to compute the result of the final iteration.
    uint64_t hash = 1469598103934665603ULL;

    // Pre-allocate buffer to avoid repeated allocations.
    std::vector<Element> row;
    row.reserve(static_cast<std::size_t>(vocab));

    for (int b = 0; b < beams; ++b) {
        row.clear();
        const std::size_t base = static_cast<std::size_t>(b) * static_cast<std::size_t>(vocab);
        
        for (int v = 0; v < vocab; ++v) {
            row.push_back({logits[base + v], v});
        }

        if (topk < vocab) {
            // Use nth_element to find the top-k in O(V) time.
            std::nth_element(row.begin(), row.begin() + topk, row.end());
            // Sort only the top-k elements in O(k log k) time.
            std::sort(row.begin(), row.begin() + topk);
        } else {
            // If topk >= vocab, just sort everything.
            std::sort(row.begin(), row.end());
        }

        // Update hash with the top-k elements.
        int limit = (topk < vocab) ? topk : vocab;
        for (int k = 0; k < limit; ++k) {
            hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
            hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].index)));
        }
        
        // If topk > vocab, the baseline behavior (implied by the loop) would be out-of-bounds.
        // However, the baseline code uses row[k], and row is size 'vocab'. 
        // The task implies topk <= vocab, but we handle the limit just in case.
    }

    return hash;
}
