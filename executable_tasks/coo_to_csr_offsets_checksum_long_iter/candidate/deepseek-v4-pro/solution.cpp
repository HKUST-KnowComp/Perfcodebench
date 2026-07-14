#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    // Compute CSR offsets (size rows + 1)
    std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0u);
    for (uint32_t r : row_ids) {
        // sorted input ensures sequential access; increment count for row r
        ++offsets[static_cast<std::size_t>(r) + 1u];
    }
    // prefix sum to convert counts to offsets
    for (std::size_t i = 1u; i < offsets.size(); ++i) {
        offsets[i] += offsets[i - 1u];
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (uint32_t v : offsets) {
            hash ^= v;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
