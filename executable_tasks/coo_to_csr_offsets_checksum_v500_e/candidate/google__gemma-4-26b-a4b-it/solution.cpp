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
        uint64_t hash = 1469598103934665603ULL;
        // If rows is 0, offsets array is size 1 (just the 0 offset)
        hash = mix(hash, 0);
        return hash;
    }

    // Since row_ids is sorted, we can build offsets in O(N + rows)
    // Step 1: Count frequencies using a dense array for speed
    std::vector<uint32_t> counts(rows, 0);
    for (uint32_t row : row_ids) {
        if (row < rows) {
            counts[row]++;
        }
    }

    // Step 2: Compute CSR offsets
    // offsets[i+1] = offsets[i] + counts[i]
    std::vector<uint32_t> offsets(static_cast<size_t>(rows) + 1);
    offsets[0] = 0;
    for (uint32_t i = 0; i < rows; ++i) {
        offsets[i + 1] = offsets[i] + counts[i];
    }

    // Step 3: Perform the hashing loop
    // The hash calculation is deterministic and depends only on the offsets.
    // The baseline code re-calculates the same hash 'iters' times.
    // However, the baseline code resets 'hash' to the seed inside the loop.
    // This means the final result is just the hash of the offsets calculated once.
    
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
        hash = mix(hash, static_cast<uint64_t>(v));
    }

    // Note: The baseline code structure: 
    // for (iter < iters) { ... hash = seed; for (v : offsets) hash = mix(hash, v); }
    // This means the loop doesn't accumulate hash across iterations, 
    // it just overwrites it. Thus, we only need to run it once.
    // If iters is 0, the baseline returns 0 (initial value of hash).
    if (iters <= 0) return 0;

    return hash;
}
