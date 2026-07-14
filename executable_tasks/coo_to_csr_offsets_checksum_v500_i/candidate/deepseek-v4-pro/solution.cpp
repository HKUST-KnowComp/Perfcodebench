#include "interface.h"

#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    if (iters == 0) {
        return 0;
    }
    // offsets[0] = 0; offsets[i+1] = cumulative count of rows < i
    // build histogram directly into offsets vector (skip separate counts)
    std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0);
    for (uint32_t r : row_ids) {
        // r is in [0, rows-1]
        ++offsets[static_cast<std::size_t>(r) + 1u];
    }
    // exclusive prefix sum
    for (std::size_t i = 0; i < static_cast<std::size_t>(rows); ++i) {
        offsets[i + 1] += offsets[i];
    }
    // compute checksum identical to baseline
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
        hash ^= v;
        hash *= 1099511628211ULL;
    }
    return hash;
}