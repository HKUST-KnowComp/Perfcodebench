#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (iters <= 0) {
        return 0;
    }

    uint64_t hash = 1469598103934665603ULL;
    std::size_t n = row_ptr.size();
    if (n < 2) {
        return hash;
    }

    const uint32_t* r_ptr = row_ptr.data();
    const uint32_t* v_ptr = values.data();
    std::size_t num_rows = n - 1;

    for (std::size_t r = 0; r < num_rows; ++r) {
        uint32_t start = r_ptr[r];
        uint32_t end = r_ptr[r + 1];
        uint64_t sum = 0;
        for (uint32_t i = start; i < end; ++i) {
            sum += v_ptr[i];
        }
        hash ^= sum;
        hash *= 1099511628211ULL;
    }

    return hash;
}
