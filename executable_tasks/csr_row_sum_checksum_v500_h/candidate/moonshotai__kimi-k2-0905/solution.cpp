#include "interface.h"

#include <vector>
#include <cstddef>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

inline constexpr std::size_t UNROLL = 4;

} // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters) {
    uint64_t hash = 0;
    const std::size_t nrows = row_ptr.size() - 1;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;

        // Process rows in SWAR fashion to pipeline sum calculation
        for (std::size_t r = 0; r < nrows; ++r) {
            auto begin = static_cast<const uint32_t*>(values.data() + row_ptr[r]);
            auto end   = static_cast<const uint32_t*>(values.data() + row_ptr[r + 1]);
            std::size_t len = end - begin;

            uint64_t sum = 0;
            const std::size_t main_len = len & ~(UNROLL - 1);
            const std::size_t tail_len = len - main_len;

            // Unrolled body
            for (std::size_t i = 0; i < main_len; i += UNROLL) {
                sum += static_cast<uint64_t>(begin[i + 0]);
                sum += static_cast<uint64_t>(begin[i + 1]);
                sum += static_cast<uint64_t>(begin[i + 2]);
                sum += static_cast<uint64_t>(begin[i + 3]);
            }

            // Tail loop (0-3 elements)
            for (std::size_t i = main_len; i < len; ++i) {
                sum += static_cast<uint64_t>(begin[i]);
            }

            hash = mix(hash, sum);
        }
    }
    return hash;
}