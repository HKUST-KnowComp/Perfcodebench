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
    if (iters <= 0) return 0;

    // Step 1: Compute offsets in O(N + rows) time.
    // Since row_ids is sorted, we can count occurrences efficiently.
    std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1, 0);
    
    // Count frequencies using a direct array to avoid map overhead
    // We use the fact that row_ids is sorted to build offsets directly.
    // However, a simple frequency count is more robust if 'sorted' implies non-decreasing.
    // Given the task 'Convert sorted COO row ids', we can use a single pass.
    
    for (uint32_t row : row_ids) {
        if (row < rows) {
            offsets[row + 1]++;
        }
    }

    // Prefix sum to transform counts into CSR offsets
    for (uint32_t i = 0; i < rows; ++i) {
        offsets[i + 1] += offsets[i];
    }

    // Step 2: Perform the hashing 'iters' times.
    // The offsets array is constant across iterations.
    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        // The baseline logic re-initializes hash inside the loop.
        // Looking at the baseline: hash = 1469598103934665603ULL; is inside the loop.
        // This means the final hash is just the hash of the offsets array.
        // If iters > 1, the loop effectively just repeats the same calculation.
        // We must match the baseline behavior exactly.
        hash = 1469598103934665603ULL;
        for (uint32_t v : offsets) {
            hash = mix(hash, static_cast<uint64_t>(v));
        }
    }

    return hash;
}
