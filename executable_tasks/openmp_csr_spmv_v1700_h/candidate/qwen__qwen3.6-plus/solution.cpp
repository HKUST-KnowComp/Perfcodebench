#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

uint64_t csr_spmv_checksum(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<int32_t>& values,
    const std::vector<int32_t>& dense,
    int rows,
    int iters) {
    
    const int* rp = row_ptr.data();
    const int* ci = col_idx.data();
    const int32_t* val = values.data();
    const int32_t* d = dense.data();
    std::vector<int64_t> out(rows);
    int64_t* o = out.data();

    #pragma omp parallel
    {
        for (int iter = 0; iter < iters; ++iter) {
            #pragma omp for schedule(static)
            for (int r = 0; r < rows; ++r) {
                int64_t sum = 0;
                int start = rp[r];
                int end = rp[r+1];
                for (int i = start; i < end; ++i) {
                    sum += static_cast<int64_t>(val[i]) * d[ci[i]];
                }
                o[r] = sum;
            }
        }
    }

    uint64_t hash = 1469598103934665603ULL;
    for (int i = 0; i < rows; ++i) {
        hash ^= static_cast<uint64_t>(o[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}