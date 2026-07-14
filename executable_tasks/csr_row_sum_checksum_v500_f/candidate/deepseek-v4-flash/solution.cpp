#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    uint64_t hash = 0;
    const std::size_t num_rows = row_ptr.size() - 1;
    const uint32_t* row_ptr_data = row_ptr.data();
    const uint32_t* values_data = values.data();

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t r = 0; r < num_rows; ++r) {
            uint64_t sum = 0;
            uint32_t start = row_ptr_data[r];
            uint32_t end = row_ptr_data[r + 1];
            for (uint32_t j = start; j < end; ++j) {
                sum += static_cast<uint64_t>(values_data[j]);
            }
            hash = mix(hash, sum);
        }
    }
    return hash;
}
