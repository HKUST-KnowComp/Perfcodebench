#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <cstdint>

namespace {

// Tie-breaking: higher score first, then lower index first.
// To use std::nth_element/std::sort correctly for the checksum,
// we define a strict weak ordering that matches the 'better' logic.
struct Token {
    int32_t score;
    int32_t id;

    // Returns true if 'a' is better than 'b'
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
    
    // Pre-allocate buffer to avoid repeated allocations
    std::vector<Token> row(static_cast<size_t>(vocab));
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        
        for (int b = 0; b < beams; ++b) {
            const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
            
            // Load logits into the Token buffer
            for (int v = 0; v < vocab; ++v) {
                row[v] = {logits[base + v], v};
            }

            // Use nth_element to find the top-k elements in O(V) average time
            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end());
                // The first topk elements are the best, but not necessarily sorted.
                // We must sort only the topk elements to ensure deterministic checksum.
                std::sort(row.begin(), row.begin() + topk);
            } else {
                // If topk >= vocab, we just sort everything
                std::sort(row.begin(), row.end());
            }

            // Accumulate hash
            int limit = std::min(topk, vocab);
            for (int k = 0; k < limit; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].id)));
            }
        }
        final_hash = hash;
    }

    return final_hash;
}
