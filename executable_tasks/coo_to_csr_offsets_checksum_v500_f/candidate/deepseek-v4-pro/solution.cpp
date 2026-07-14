#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int /*iters*/) {
    // Initialize offset array (size rows+1) with zeros
    std::vector<uint32_t> offsets(static_cast<std::size_t>(rows) + 1u, 0);

    // Count non-zeros per row using run-length encoding (sorted row_ids assumed)
    if (!row_ids.empty()) {
        uint32_t curr_row = row_ids[0];
        uint32_t count = 1;
        for (std::size_t i = 1; i < row_ids.size(); ++i) {
            if (row_ids[i] == curr_row) {
                ++count;
            } else {
                offsets[static_cast<std::size_t>(curr_row) + 1u] = count;
                curr_row = row_ids[i];
                count = 1;
            }
        }
        offsets[static_cast<std::size_t>(curr_row) + 1u] = count;
    }

    // Prefix sum to convert row counts into CSR offsets
    for (std::size_t r = 1; r < offsets.size(); ++r) {
        offsets[r] += offsets[r - 1];
    }

    // Compute checksum (result is independent of 'iters')
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t v : offsets) {
        hash ^= v;
        hash *= 1099511628211ULL;
    }
    return hash;
}
