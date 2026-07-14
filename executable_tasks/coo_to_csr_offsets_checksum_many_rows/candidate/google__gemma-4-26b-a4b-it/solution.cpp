#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    if (rows == 0) {
        // Handle edge case where rows might be 0, though unlikely per task description
        uint64_t hash = 1469598103934665603ULL;
        return hash;
    }

    // Pre-allocate buffers to avoid repeated allocations inside the loop
    std::vector<uint32_t> counts(rows, 0);
    std::vector<uint32_t> offsets(rows + 1, 0);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // Reset counts
        std::fill(counts.begin(), counts.end(), 0);

        // Count occurrences: O(N)
        for (uint32_t row : row_ids) {
            if (row < rows) {
                counts[row]++;
            }
        }

        // Compute CSR offsets: O(R)
        offsets[0] = 0;
        for (uint32_t r = 0; r < rows; ++r) {
            offsets[r + 1] = offsets[r] + counts[r];
        }

        // Compute hash: O(R)
        hash = 1469598103934665603ULL;
        for (uint32_t v : offsets) {
            hash = mix(hash, static_cast<uint64_t>(v));
        }
    }

    return hash;
}