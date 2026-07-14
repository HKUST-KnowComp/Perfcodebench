#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
    // Preallocate the offsets array once (size = rows + 1)
    std::vector<uint32_t> offsets(rows + 1, 0);
    const uint32_t* data = row_ids.data();
    std::size_t n = row_ids.size();

    uint64_t hash = 1469598103934665603ULL;  // initial value (reset each iteration)

    for (int iter = 0; iter < iters; ++iter) {
        offsets[0] = 0;
        std::size_t idx = 0;

        // Build CSR offsets in a single pass over rows
        for (uint32_t r = 0; r < rows; ++r) {
            uint32_t cnt = 0;
            while (idx < n && data[idx] == r) {
                ++cnt;
                ++idx;
            }
            offsets[static_cast<std::size_t>(r) + 1] = offsets[static_cast<std::size_t>(r)] + cnt;
        }

        // Compute FNV-1a hash of the offsets array
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i <= rows; ++i) {
            hash ^= offsets[i];
            hash *= 1099511628211ULL;
        }
    }

    return hash;
}
