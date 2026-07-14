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
    const uint32_t* ptr_ptr = row_ptr.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t r = 0; r < num_rows; ++r) {
            uint64_t sum = 0;
            const uint32_t start = ptr_ptr[r];
            const uint32_t end = ptr_ptr[r + 1];
            
            for (uint32_t i = start; i < end; ++i) {
                sum += val_ptr[i];
            }
            hash = mix(hash, sum);
        }
    }
    return hash;
}