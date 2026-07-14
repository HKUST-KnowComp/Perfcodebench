#include "interface.h"
#include <cstdint>
#include <vector>
#include <omp.h>

namespace {

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
    uint64_t hash = 1469598103934665603ULL;
    const uint32_t* ptr = data.data();
    size_t n = data.size();
    for (size_t i = 0; i < n; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t jacobi_checksum(const std::vector<uint32_t>& input, int rows, int cols, int steps) {
    if (input.empty()) return 0;

    std::vector<uint32_t> a = input;
    std::vector<uint32_t> b = input; // Initialize b with input to handle boundaries correctly
    
    uint32_t* p_a = a.data();
    uint32_t* p_b = b.data();
    const size_t c_size = static_cast<size_t>(cols);
    const size_t r_size = static_cast<size_t>(rows);

    for (int step = 0; step < steps; ++step) {
        #pragma omp parallel for schedule(static)
        for (int r = 1; r < rows - 1; ++r) {
            const size_t row_offset = static_cast<size_t>(r) * c_size;
            const size_t prev_row = row_offset - c_size;
            const size_t next_row = row_offset + c_size;

            // Process inner columns (1 to cols-2)
            for (size_t c = 1; c < (size_t)cols - 1; ++c) {
                const size_t idx = row_offset + c;
                p_b[idx] = (p_a[idx] * 4u + 
                            p_a[idx - 1] + 
                            p_a[idx + 1] + 
                            p_a[prev_row + c] + 
                            p_a[next_row + c]) / 8u;
            }
        }

        // Boundary conditions are preserved because p_b was initialized with p_a values
        // and we only update the interior. However, since we swap, we must ensure 
        // the boundaries of the 'new' a are correct. 
        // The simplest way is to ensure the boundary of b matches a at the start of each step.
        // But since we only write to the interior of b, and b starts as a copy of a, 
        // the boundaries of b remain identical to a's boundaries.
        
        // To handle the swap correctly for the next iteration's boundary check:
        // We need to ensure the boundaries of the 'new' a (which was b) are correct.
        // Since we only update interior of b, b's boundaries are already correct.
        
        // We must ensure that the boundaries of the 'new' a are updated if they were to change.
        // In this specific Jacobi, boundaries are constant: b[boundary] = a[boundary].
        // So we just swap.
        
        // Optimization: Instead of full copy, we only need to ensure boundaries are consistent.
        // Since boundaries never change, we can just swap pointers.
        std::swap(p_a, p_b);
        // Note: std::swap on raw pointers doesn't work for the vector contents, 
        // but we are swapping the local pointers p_a and p_b which point to the vector data.
        // However, we need to swap the actual underlying data or the pointers themselves.
        // Let's use the vector swap to be safe and simple.
        a.swap(b);
        p_a = a.data();
        p_b = b.data();
    }

    return checksum_u32(a);
}
