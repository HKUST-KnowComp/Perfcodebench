#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace {

// Tie-breaking rule: higher score first, then lower index first.
// To use std::nth_element/std::sort correctly with the 'better' logic,
// we define a strict weak ordering where 'a' is 'less than' 'b' if 'b' is better.
struct Token {
    int32_t score;
    int32_t id;

    // We want the 'best' elements at the beginning of the range.
    // For nth_element to put the top-k at the front, we define a comparator 
    // such that 'better' elements are 'less' than 'worse' elements.
    bool operator<(const Token& other) const {
        if (score != other.score) {
            return score > other.score;
        }
        return id < other.id;
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
    
    // Use a single buffer to avoid repeated allocations
    std::vector<Token> row(static_cast<size_t>(vocab));
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
            
            // Load data into the buffer
            for (int v = 0; v < vocab; ++v) {
                row[v] = {logits[base + v], v};
            }

            // Use nth_element to find the top-k in O(V) average time
            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end());
                // The top-k are now in [begin, begin + topk), but not necessarily sorted.
                // The checksum requires deterministic order (the top-k must be sorted).
                std::sort(row.begin(), row.begin() + topk);
            } else {
                // If topk >= vocab, we just sort everything
                std::sort(row.begin(), row.end());
            }

            // Calculate hash for the top-k elements
            int actual_k = std::min(topk, vocab);
            for (int k = 0; k < actual_k; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
                hash = mix(hash, static_cast<uint64_t>(row[k].id));
            }
        }
    }
    return hash;
}
