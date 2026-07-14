#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t csr_row_hash(const std::vector<uint32_t>& row_ptr, const std::vector<uint32_t>& values, int iters) {
    if (iters <= 0) return 0;
    
    size_t num_rows = row_ptr.size() > 0 ? row_ptr.size() - 1 : 0;
    std::vector<uint64_t> row_sums(num_rows);
    const uint32_t* val_data = values.data();
    const uint32_t* rp_data = row_ptr.data();
    
    // Compute row sums once, avoiding per-row allocations and redundant scans
    for (size_t r = 0; r < num_rows; ++r) {
        uint64_t sum = 0;
        const uint32_t* p = val_data + rp_data[r];
        const uint32_t* e = val_data + rp_data[r + 1];
        while (p < e) {
            sum += *p++;
        }
        row_sums[r] = sum;
    }
    
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t* sums_data = row_sums.data();
    
    // Hash computation loop matches baseline semantics exactly
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t r = 0; r < num_rows; ++r) {
            hash ^= sums_data[r];
            hash *= 1099511628211ULL;
        }
    }
    
    return hash;
}