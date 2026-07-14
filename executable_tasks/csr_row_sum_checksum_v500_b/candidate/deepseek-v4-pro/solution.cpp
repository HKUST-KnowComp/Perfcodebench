#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (iters == 0) return 0;

    const uint32_t* rp = row_ptr.data();
    const uint32_t* vals = values.data();
    const size_t nrows = row_ptr.size() - 1;

    uint64_t hash = 1469598103934665603ULL;
    for (size_t r = 0; r < nrows; ++r) {
        uint32_t start = rp[r];
        uint32_t end   = rp[r + 1];
        uint64_t sum = 0;
        for (size_t i = start; i < end; ++i) {
            sum += static_cast<uint64_t>(vals[i]);
        }
        hash ^= sum;
        hash *= 1099511628211ULL;
    }

    return hash;
}
