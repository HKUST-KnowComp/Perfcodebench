#include "interface.h"
#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    uint64_t hash = 0;
    const size_t num_rows = row_ptr.size() - 1;
    const uint32_t* val_ptr = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t r = 0; r < num_rows; ++r) {
            uint64_t row_sum = 0;
            const size_t start = static_cast<size_t>(row_ptr[r]);
            const size_t end = static_cast<size_t>(row_ptr[r + 1]);
            
            for (size_t i = start; i < end; ++i) {
                row_sum += static_cast<uint64_t>(val_ptr[i]);
            }
            hash = mix(hash, row_sum);
        }
    }
    return hash;
}