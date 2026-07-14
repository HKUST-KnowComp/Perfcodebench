#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

// Parallelized checksum using OpenMP reduction
uint64_t checksum_u32(const std::vector<uint32_t>& data) {
    uint64_t global_hash = 1469598103934665603ULL;
    
    // We use a custom reduction or a simple serial loop if the overhead is too high,
    // but for large grids, parallelizing the hash can help. 
    // However, the FNV-1a hash is inherently sequential. 
    // To keep correctness and simplicity, we'll use a serial checksum as the bottleneck is the Jacobi steps.
    for (uint32_t v : data) {
        global_hash ^= static_cast<uint64_t>(v);
        global_hash *= 1099511628211ULL;
    }
    return global_hash;
}

} // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
    if (input.empty()) return 0;

    std::vector<uint32_t> a = input;
    std::vector<uint32_t> b = input; // Initialize b with input to handle boundaries correctly
    
    const std::size_t n = static_cast<std::size_t>(rows) * static_cast<std::size_t>(cols);
    const std::size_t c_size = static_cast<std::size_t>(cols);

    for (int step = 0; step < steps; ++step) {
        // Parallelize the interior update. 
        // We skip the first and last rows/cols to avoid boundary checks inside the loop.
        #pragma omp parallel for schedule(static)
        for (int row = 1; row < rows - 1; ++row) {
            const std::size_t row_offset = static_cast<std::size_t>(row) * c_size;
            for (int col = 1; col < cols - 1; ++col) {
                const std::size_t idx = row_offset + col;
                // Jacobi update formula
                b[idx] = (a[idx] * 4u + 
                          a[idx - 1] + 
                          a[idx + 1] + 
                          a[idx - c_size] + 
                          a[idx + c_size]) / 8u;
            }
        }
        // Note: Boundaries in 'b' are already set to 'a' values from the initialization/previous step
        // because the boundary cells (row 0, row rows-1, col 0, col cols-1) are never updated in the loop.
        // However, we must ensure 'b' boundary values match 'a' boundary values at the start of each step.
        // Since the boundary values never change in this specific Jacobi variant, 
        // we can just initialize 'b' once and only update the interior.
        
        // To ensure correctness if boundaries were to change (though they don't here):
        // We rely on the fact that b[boundary] = a[boundary] is constant.
        
        a.swap(b);
    }

    return checksum_u32(a);
}
