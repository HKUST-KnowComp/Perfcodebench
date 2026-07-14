#include "interface.h"
#include <vector>
#include <cstddef>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters) noexcept {
    const uint32_t* __restrict rp = row_ptr.data();
    const uint32_t* __restrict vp = values.data();
    const std::size_t rows = row_ptr.size() - 1;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t r = 0; r < rows; ++r) {
            const uint32_t start = rp[r];
            const uint32_t end   = rp[r + 1];
            uint64_t sum = 0;
            for (uint32_t j = start; j < end; ++j) {
                sum += vp[j];
            }
            hash = mix(hash, sum);
        }
    }
    return hash;
}