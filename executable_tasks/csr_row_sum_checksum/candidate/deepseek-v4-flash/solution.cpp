#include "interface.h"

#include <vector>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    std::size_t num_rows = row_ptr.empty() ? 0 : row_ptr.size() - 1;
    std::vector<uint64_t> row_sums(num_rows);

    const uint32_t* row_ptr_data = row_ptr.data();
    const uint32_t* values_data = values.data();

    for (std::size_t r = 0; r < num_rows; ++r) {
        std::size_t start = static_cast<std::size_t>(row_ptr_data[r]);
        std::size_t end = static_cast<std::size_t>(row_ptr_data[r + 1]);
        uint64_t sum = 0;
        for (std::size_t j = start; j < end; ++j) {
            sum += static_cast<uint64_t>(values_data[j]);
        }
        row_sums[r] = sum;
    }

    const uint64_t* sums_data = row_sums.data();
    uint64_t hash = 0;
    constexpr uint64_t init_hash = 1469598103934665603ULL;
    constexpr uint64_t magic = 1099511628211ULL;
    for (int iter = 0; iter < iters; ++iter) {
        hash = init_hash;
        for (std::size_t r = 0; r < num_rows; ++r) {
            uint64_t sum = sums_data[r];
            hash ^= sum;
            hash *= magic;
        }
    }
    return hash;
}
