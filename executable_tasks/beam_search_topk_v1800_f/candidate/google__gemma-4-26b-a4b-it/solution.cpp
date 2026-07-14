#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <cstdint>

namespace {

struct Entry {
    int32_t score;
    int32_t id;

    // Tie-breaking: higher score first, then lower id first
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
    if (beams <= 0 || vocab <= 0 || topk <= 0) return 0;
    int actual_topk = std::min(topk, vocab);

    // Pre-allocate buffer to avoid repeated allocations
    std::vector<Entry> row(static_cast<size_t>(vocab));
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
            
            // Load data into the entry buffer
            for (int v = 0; v < vocab; ++v) {
                row[v] = {logits[base + v], v};
            }

            // Use nth_element to find the top-k elements in O(V) average time
            std::nth_element(row.begin(), row.begin() + actual_topk, row.end(), std::less<Entry>());
            
            // Sort only the top-k elements to ensure deterministic order for checksum
            std::sort(row.begin(), row.begin() + actual_topk, std::less<Entry>());

            for (int k = 0; k < actual_topk; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].score)));
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].id)));
            }
        }
    }
    return hash;
}
