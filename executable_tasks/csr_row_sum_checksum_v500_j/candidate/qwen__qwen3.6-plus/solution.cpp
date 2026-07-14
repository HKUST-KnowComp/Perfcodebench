#include "interface.h"
#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (iters <= 0) return 0;

    const size_t num_rows = row_ptr.empty() ? 0 : row_ptr.size() - 1;
    std::vector<uint64_t> row_sums;
    row_sums.reserve(num_rows);

    const uint32_t* rp = row_ptr.data();
    const uint32_t* vp = values.data();

    for (size_t r = 0; r < num_rows; ++r) {
        uint64_t sum = 0;
        const uint32_t* v = vp + rp[r];
        const uint32_t* vend = vp + rp[r + 1];
        while (v < vend) {
            sum += *v++;
        }
        row_sums.push_back(sum);
    }

    uint64_t hash = 0;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (uint64_t s : row_sums) {
            hash ^= s;
            hash *= FNV_PRIME;
        }
    }
    return hash;
}