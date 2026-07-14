#include "interface.h"

#include <algorithm>
#include <utility>
#include <vector>
#include <cstdint>

namespace {

struct Entry {
    int32_t val;
    int32_t idx;

    // Custom comparator for deterministic tie-breaking:
    // Higher value is better; if values are equal, lower index is better.
    bool operator>(const Entry& other) const {
        if (val != other.val) return val > other.val;
        return idx < other.idx;
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
    std::vector<Entry> row(vocab);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < beams; ++b) {
            const int32_t* beam_logits = &logits[static_cast<size_t>(b) * vocab];
            
            // Fill the row buffer with value-index pairs
            for (int v = 0; v < vocab; ++v) {
                row[v] = {beam_logits[v], v};
            }

            // Use nth_element to find the top-k elements in O(V) average time
            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end(), std::greater<Entry>());
            }

            // Sort only the top-k elements to ensure deterministic order for hashing
            int actual_k = (topk < vocab) ? topk : vocab;
            std::sort(row.begin(), row.begin() + actual_k, std::greater<Entry>());

            // Update hash with the top-k results
            for (int k = 0; k < actual_k; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].val)));
                hash = mix(hash, static_cast<uint64_t>(row[k].idx));
            }
        }
    }
    return hash;
}
