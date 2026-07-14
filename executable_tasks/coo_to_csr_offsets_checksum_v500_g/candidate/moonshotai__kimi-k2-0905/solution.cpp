#include "interface.h"
#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    // One counter array reused across iterations
    std::vector<uint32_t> cnt(rows, 0u);
    std::vector<uint32_t> offsets(rows + 1, 0u);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        // Clear counters quickly
        memset(cnt.data(), 0, rows * sizeof(uint32_t));

        // Count occurrences of each row ID
        for (uint32_t rid : row_ids) {
            ++cnt[rid];
        }

        // Exclusive scan to produce CSR offsets
        uint32_t off = 0;
        for (uint32_t r = 0; r < rows; ++r) {
            offsets[r] = off;
            off += cnt[r];
        }
        offsets[rows] = off;

        // Compute hash of offset array
        hash = 1469598103934665603ULL;
        for (uint32_t v : offsets) {
            hash = mix(hash, v);
        }
    }
    return hash;
}