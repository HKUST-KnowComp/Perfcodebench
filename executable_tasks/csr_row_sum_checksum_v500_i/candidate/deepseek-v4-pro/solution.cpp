#include "interface.h"

#include <cstddef>
#include <vector>

namespace {

constexpr uint64_t base_hash = 1469598103934665603ULL;
constexpr uint64_t mix_mult  = 1099511628211ULL;

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= mix_mult;
    return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr,
                      const std::vector<uint32_t>& values,
                      int iters) {
    if (iters <= 0) {
        return 0;
    }

    const std::size_t num_rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
    if (num_rows == 0) {
        return base_hash;
    }

    const uint32_t* row_ptr_data = row_ptr.data();
    const uint32_t* values_data = values.data();

    std::vector<uint64_t> row_sums(num_rows);

    for (std::size_t r = 0; r < num_rows; ++r) {
        const uint32_t start = row_ptr_data[r];
        const uint32_t end   = row_ptr_data[r + 1];
        const uint32_t* p    = values_data + start;
        const uint32_t* pe   = values_data + end;
        uint64_t sum = 0;
        for (; p < pe; ++p) {
            sum += *p;
        }
        row_sums[r] = sum;
    }

    const uint64_t* sums_data = row_sums.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = base_hash;
        for (std::size_t r = 0; r < num_rows; ++r) {
            hash = mix(hash, sums_data[r]);
        }
    }

    return hash;
}
