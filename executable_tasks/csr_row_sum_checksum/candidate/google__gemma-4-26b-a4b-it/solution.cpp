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
    if (row_ptr.empty()) return 0;
    
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const size_t num_rows = row_ptr.size() - 1;
    const uint32_t* val_ptr = values.data();
    const uint32_t* ptr_data = row_ptr.data();

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET_BASIS;
        for (size_t r = 0; r < num_rows; ++r) {
            uint64_t sum = 0;
            const uint32_t start = ptr_data[r];
            const uint32_t end = ptr_data[r + 1];
            
            // Direct accumulation without vector allocation
            for (uint32_t i = start; i < end; ++i) {
                sum += val_ptr[i];
            }
            hash = mix(hash, sum);
        }
        final_hash = hash;
    }

    return final_hash;
}