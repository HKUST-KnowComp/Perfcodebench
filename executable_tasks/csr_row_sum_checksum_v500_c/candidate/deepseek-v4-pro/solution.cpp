#include "interface.h"
#include <cstdint>
#include <vector>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}
}

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    // Precompute per-row sums
    const std::size_t num_rows = row_ptr.empty() ? 0 : row_ptr.size() - 1;
    std::vector<uint64_t> row_sums(num_rows);
    for (std::size_t r = 0; r < num_rows; ++r) {
        uint64_t sum = 0;
        const std::size_t start = row_ptr[r];
        const std::size_t end = row_ptr[r + 1];
        for (std::size_t idx = start; idx < end; ++idx) {
            sum += static_cast<uint64_t>(values[idx]);
        }
        row_sums[r] = sum;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t r = 0; r < num_rows; ++r) {
            hash = mix(hash, row_sums[r]);
        }
    }
    return hash;
}
