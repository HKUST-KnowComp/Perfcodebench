#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

/**
 * Parallelized version of the FNV-1a-like hash function provided in the baseline.
 * Since the hash is sequential in nature, we compute the final vector first
 * and then hash it. To speed up the hash, we ensure the SpMV is fast.
 */
uint64_t checksum_i64(const std::vector<int64_t>& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (int64_t v : data) {
        hash ^= static_cast<uint64_t>(v);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t csr_spmv_checksum(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<int32_t>& values,
    const std::vector<int32_t>& dense,
    int rows,
    int iters) {
    
    if (rows <= 0) return 1469598103934665603ULL;

    std::vector<int64_t> out(static_cast<std::size_t>(rows));

    // Pointers to raw data for faster access and to assist compiler vectorization
    const int* r_ptr = row_ptr.data();
    const int* c_idx = col_idx.data();
    const int32_t* vals = values.data();
    const int32_t* d_vec = dense.data();
    int64_t* o_vec = out.data();

    // Parallelize the SpMV operation.
    // We keep the parallel region outside the iters loop to minimize fork/join overhead.
    #pragma omp parallel
    {
        for (int iter = 0; iter < iters; ++iter) {
            // Guided scheduling is effective for CSR where row lengths (work per row) vary.
            #pragma omp for schedule(guided)
            for (int row = 0; row < rows; ++row) {
                int64_t sum = 0;
                int start = r_ptr[row];
                int end = r_ptr[row + 1];
                
                // Inner loop: Dot product of sparse row and dense vector.
                // The compiler is encouraged to vectorize this loop.
                #pragma omp simd reduction(+:sum)
                for (int idx = start; idx < end; ++idx) {
                    sum += (int64_t)vals[idx] * (int64_t)d_vec[c_idx[idx]];
                }
                o_vec[row] = sum;
            }
        }
    }

    return checksum_i64(out);
}
