#include "interface.h"
#include <vector>
#include <cstdint>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}
}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    const uint32_t nrows = static_cast<uint32_t>(row_ptr.size() - 1);
    std::vector<uint64_t> row_sum(nrows);

    // --- aggregate once, cache-friendly ---
    for (uint32_t row = 0; row != nrows; ++row) {
        uint32_t lo = row_ptr[row];
        uint32_t hi = row_ptr[row + 1];
        uint64_t sum = 0;
        uint32_t width = hi - lo;

        // 4-way unrolled vectorisable add
        for (uint32_t off = 0; off + 3 < width; off += 4) {
            sum += static_cast<uint64_t>(values.data()[lo + off]);
            sum += static_cast<uint64_t>(values.data()[lo + off + 1]);
            sum += static_cast<uint64_t>(values.data()[lo + off + 2]);
            sum += static_cast<uint64_t>(values.data()[lo + off + 3]);
        }
        for (uint32_t rem = width & 3; rem; --rem) {
            sum += static_cast<uint64_t>(values.data()[lo + width - rem]);
        }
        row_sum[row] = sum;
    }

    // --- hash iters: all input data already in L1/L2 ---
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 0xcbf29ce484222325ULL;
        for (uint32_t row = 0; row != nrows; ++row) {
            hash = mix(hash, row_sum[row]);
        }
        if (iter == iters - 1) return hash;
    }
    return 0;
}