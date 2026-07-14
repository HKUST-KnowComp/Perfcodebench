#include "interface.h"
#include "is_utf8.h"
#include <cstddef>
#include <string>
#include <vector>

/**
 * Performance Engineer's Note:
 * The goal is to validate a batch of UTF-8 strings efficiently.
 * The baseline uses a branch-heavy scalar loop. 
 * We replace this with a call to a specialized UTF-8 validation function 
 * (likely SIMD-accelerated) provided by the environment's is_utf8.h.
 */

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    
    // Process strings in the batch.
    // Most high-performance UTF-8 libraries (like simdutf or similar) 
    // provide a function to validate a buffer.
    for (const auto& s : inputs) {
        const char* data = s.data();
        std::size_t len = s.size();
        
        // is_utf8 is assumed to be the optimized entry point from is_utf8.h
        // If the signature differs slightly (e.g., unsigned char*), we cast.
        if (is_utf8(data, len)) {
            count++;
        }
    }
    
    return count;
}
