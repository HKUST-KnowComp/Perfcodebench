#include "interface.h"

#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    uint64_t hash = 0;
    const uint32_t* val_data = values.data();
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t r = 0; r + 1 < row_ptr.size(); ++r) {
            uint64_t sum = 0;
            const uint32_t* start = val_data + row_ptr[r];
            const uint32_t* end = val_data + row_ptr[r + 1];
            while (start < end) {
                sum += *start++;
            }
            hash ^= sum;
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}