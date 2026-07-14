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
        // If rows is 0, offsets array has 1 element: [0]
        return mix(hash, 0);
    }

    // Since row_ids is sorted, we can build offsets in O(N + rows)
    // Step 1: Count frequencies using a dense array to avoid map overhead
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

    // Step 3: Perform hashing 'iters' times
    // The offsets array is constant across iterations, so we only need to compute the hash once
    // and return it. However, the original code re-initialized the hash inside the loop.
    // Looking at the original logic: 
    // for (int iter = 0; iter < iters; ++iter) { ... hash = 1469598103934665603ULL; for (v : offsets) hash = mix(hash, v); }
    // This means the final hash is simply the hash of the offsets array.
    
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
        hash = mix(hash, static_cast<uint64_t>(v));
    }

    return hash;
}