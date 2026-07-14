#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters)
{
    const uint32_t*  rp = row_ptr.data();
    const uint32_t*  vp = values.data();
    const std::size_t nrows = row_ptr.size() - 1;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;

        for (std::size_t r = 0; r < nrows; ++r) {
            const uint32_t* beg = vp + rp[r];
            const uint32_t* end = vp + rp[r + 1];
            uint64_t sum = 0;

            // unroll 4x
            std::size_t len = end - beg;
            std::size_t rem = len & 3;
            for (std::size_t i = 0; i < len - rem; i += 4) {
                sum += static_cast<uint64_t>(beg[i]);
                sum += static_cast<uint64_t>(beg[i + 1]);
                sum += static_cast<uint64_t>(beg[i + 2]);
                sum += static_cast<uint64_t>(beg[i + 3]);
            }
            // handle tail
            for (std::size_t i = len - rem; i < len; ++i)
                sum += static_cast<uint64_t>(beg[i]);

            hash = mix(hash, sum);
        }
    }
    return hash;
}