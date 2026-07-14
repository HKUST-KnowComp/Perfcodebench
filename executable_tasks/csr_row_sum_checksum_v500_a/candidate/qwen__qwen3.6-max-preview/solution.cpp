#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstddef>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (iters <= 0) return 0;

    uint64_t hash = 1469598103934665603ULL;
    const std::size_t num_rows = row_ptr.size();
    if (num_rows < 2) return hash;

    const uint32_t* ptr_data = row_ptr.data();
    const uint32_t* val_data = values.data();

    for (std::size_t r = 0; r + 1 < num_rows; ++r) {
        uint64_t sum = 0;
        std::size_t start = ptr_data[r];
        std::size_t end   = ptr_data[r + 1];
        for (std::size_t i = start; i < end; ++i) {
            sum += val_data[i];
        }
        hash ^= sum;
        hash *= 1099511628211ULL;
    }
    return hash;
}
