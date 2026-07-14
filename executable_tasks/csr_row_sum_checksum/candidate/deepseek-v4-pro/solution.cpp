#include "interface.h"

#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    const size_t nrows = row_ptr.empty() ? 0 : row_ptr.size() - 1;
    std::vector<uint64_t> row_sums(nrows, 0ULL);
    for (size_t r = 0; r < nrows; ++r) {
        const uint32_t* p = values.data() + static_cast<size_t>(row_ptr[r]);
        const uint32_t* pe = values.data() + static_cast<size_t>(row_ptr[r+1]);
        uint64_t sum = 0;
        while (p < pe) {
            sum += static_cast<uint64_t>(*p);
            ++p;
        }
        row_sums[r] = sum;
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (size_t r = 0; r < nrows; ++r) {
            h = mix(h, row_sums[r]);
        }
        hash = h;
    }
    return hash;
}