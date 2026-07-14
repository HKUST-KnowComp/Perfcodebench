#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

struct Entry {
    int32_t val;
    int32_t idx;

    // Custom comparator for deterministic tie-breaking: 
    // Higher value first, then lower index first.
    inline bool operator>(const Entry& other) const {
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

    // Pre-allocate buffer to avoid repeated allocations.
    std::vector<Entry> row(vocab);
    uint64_t hash = 0;

    // The baseline resets the hash every iteration, meaning only the last iteration's 
    // result is returned. However, we maintain the loop structure to ensure 
    // performance is measured correctly if the harness expects the work to be repeated.
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL; // FNV-1a offset basis

        for (int b = 0; b < beams; ++b) {
            const int32_t* row_logits = &logits[static_cast<size_t>(b) * vocab];
            
            // Fill the entry buffer.
            for (int v = 0; v < vocab; ++v) {
                row[v] = {row_logits[v], v};
            }

            // Use nth_element to find the top-k elements in O(V) time.
            // This partitions the array such that the first 'topk' elements are the largest.
            if (topk < vocab) {
                std::nth_element(row.begin(), row.begin() + topk, row.end(), std::greater<Entry>());
            }

            // Sort only the top-k elements to ensure deterministic order for the checksum.
            int actual_k = (topk < vocab) ? topk : vocab;
            std::sort(row.begin(), row.begin() + actual_k, std::greater<Entry>());

            // Update hash with the top-k results.
            for (int k = 0; k < actual_k; ++k) {
                hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(row[k].val)));
                hash = mix(hash, static_cast<uint64_t>(row[k].idx));
            }
        }
    }

    return hash;
}
