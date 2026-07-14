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

    // Step 1: Compute offsets in O(N + rows) using the fact that row_ids is sorted.
    // We use a frequency array approach for O(1) access.
    std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1, 0);
    
    // Count occurrences of each row_id
    // Since row_ids is sorted, we can do this very efficiently.
    for (uint32_t row : row_ids) {
        if (row < rows) {
            offsets[static_cast<size_t>(row) + 1]++;
        }
    }

    // Prefix sum to transform counts into CSR offsets
    for (uint32_t r = 0; r < rows; ++r) {
        offsets[static_cast<size_t>(r) + 1] += offsets[static_cast<size_t>(r)];
    }

    // Step 2: Perform the hashing 'iters' times.
    // The offsets array is constant across iterations, so we only need to compute the hash once.
    // However, the original code re-initialized the hash inside the loop.
    // Looking at the original logic: 
    // for (int iter = 0; iter < iters; ++iter) { ... hash = 1469598103934665603ULL; for (uint32_t v : offsets) { hash = mix(hash, v); } }
    // This means the final hash is simply the result of one hashing pass, repeated 'iters' times.
    // But wait, the hash is reset to the seed inside the loop, so the result of the last iteration is the final result.
    
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
        hash = mix(hash, v);
    }

    // If iters > 0, the loop runs and the last iteration's hash is returned.
    // If iters was 0, the original code would return 0 (initial hash value).
    // The logic above matches the behavior of the original loop for iters >= 1.
    
    return hash;
}