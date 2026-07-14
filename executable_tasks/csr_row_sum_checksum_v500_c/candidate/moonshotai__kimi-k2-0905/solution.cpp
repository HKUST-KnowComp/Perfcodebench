#include "interface.h"

#include <vector>
#include <cstdint>

static inline uint64_t mix(uint64_t h, uint64_t v) noexcept {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters) {
    const uint64_t* __restrict v64 = reinterpret_cast<const uint64_t*>(values.data());
    const uint32_t* __restrict ptr = row_ptr.data();
    const std::size_t rows = row_ptr.size() - 1;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (std::size_t r = 0; r < rows; ++r) {
            const uint32_t start = ptr[r];
            const uint32_t end   = ptr[r + 1];
            const uint32_t len   = end - start;

            const uint32_t* __restrict row = values.data() + start;

            uint64_t sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
            uint32_t i = 0;
            for (; i + 3 < len; i += 4) {
                sum0 += row[i];
                sum1 += row[i + 1];
                sum2 += row[i + 2];
                sum3 += row[i + 3];
            }
            uint64_t sum = sum0 + sum1 + sum2 + sum3;
            for (; i < len; ++i) {
                sum += row[i];
            }
            h = mix(h, sum);
        }
        hash = h;
    }
    return hash;
}