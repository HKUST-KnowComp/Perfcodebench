#include "interface.h"

#include <algorithm>
#include <vector>
#include <cstdint>

namespace {

struct Entry {
    int32_t score;
    int32_t id;

    // Tie-breaking: higher score first, then lower id
    bool operator<(const Entry& other) const {
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
    
    // Use a reusable buffer to avoid repeated allocations
    std::vector<Entry> row(static_cast<size_t>(vocab));
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* beam_logits = &logits[static_cast<size_t>(b) * vocab];
            
            // Fill the buffer
            for (int v = 0; v < vocab; ++v) {
                row[v] = {beam_logits[v], v};
            }

            // Partial sort to find top-k elements
            // nth_element is O(N) on average, much faster than O(N log N) sort
            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end());
                // The first topk elements are the ones we want, but they aren't sorted among themselves.
                // We must sort only the top-k to satisfy the deterministic checksum requirement.
                std::sort(row.begin(), row.begin() + topk);
            } else {
                std::sort(row.begin(), row.end());
            }

            // Accumulate hash
            for (int k = 0; k < topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].id)));
            }
        }
    }

    return hash;
}
