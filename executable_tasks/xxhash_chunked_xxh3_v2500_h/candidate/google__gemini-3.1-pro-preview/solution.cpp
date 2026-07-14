#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <string>
#include <cstddef>

/**
 * Optimized chunked XXH3 implementation.
 * 
 * The primary performance bottleneck in the baseline was the use of std::string::substr(),
 * which allocates a new string on the heap and copies the data for every chunk.
 * By passing the raw pointer and length directly to XXH3_64bits, we eliminate 
 * all allocations and unnecessary data movement.
 */
uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const char* const data = input.data();
    const std::size_t total_size = input.size();
    constexpr std::size_t kChunkSize = 1024;

    for (std::size_t offset = 0; offset < total_size; offset += kChunkSize) {
        // Calculate the size of the current chunk (handle the last partial chunk)
        const std::size_t take = (total_size - offset < kChunkSize) 
                                 ? (total_size - offset) 
                                 : kChunkSize;
        
        // Hash the chunk directly from the input buffer
        const uint64_t h = XXH3_64bits(data + offset, take);
        
        // Combine using the specified accumulator logic: XOR followed by 1-bit left rotation
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    
    return acc;
}