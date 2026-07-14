#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

} // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters) {
    const uint32_t* __restrict ptr = values.data();
    const uint32_t* __restrict rp  = row_ptr.data();
    const std::size_t rows = row_ptr.size() - 1;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t r = 0; r < rows; ++r) {
            const uint32_t* __restrict beg = ptr + rp[r];
            const uint32_t* __restrict end = ptr + rp[r + 1];

            uint64_t sum = 0;
            std::size_t len = static_cast<std::size_t>(end - beg);
            std::size_t i = 0;

            // unroll 4×
            for (; i + 4 <= len; i += 4) {
                sum += static_cast<uint64_t>(beg[i]);
                sum += static_cast<uint64_t>(beg[i + 1]);
                sum += static_cast<uint64_t>(beg[i + 2]);
                sum += static_cast<uint64_t>(beg[i + 3]);
            }
            // tail
            for (; i < len; ++i) {
                sum += static_cast<uint64_t>(beg[i]);
            }
            hash = mix(hash, sum);
        }
    }
    return hash;
}