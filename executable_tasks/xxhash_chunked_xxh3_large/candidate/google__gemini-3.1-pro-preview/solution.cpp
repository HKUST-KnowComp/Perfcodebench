#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <cstdint>
#include <string>

/**
 * Optimized chunked_hash implementation.
 * 
 * The primary optimization is removing the call to std::string::substr(), 
 * which performs a heap allocation and a memory copy for every chunk.
 * Instead, we pass the raw pointer and length directly to XXH3_64bits.
 */
uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const size_t size = input.size();
    const char* const data = input.data();
    constexpr size_t kChunkSize = 1024;

    for (size_t offset = 0; offset < size; offset += kChunkSize) {
        // Calculate the size of the current chunk
        const size_t take = (size - offset < kChunkSize) ? (size - offset) : kChunkSize;
        
        // Compute XXH3 hash for the current chunk directly from the input buffer
        const uint64_t h = XXH3_64bits(data + offset, take);
        
        // Accumulate using XOR and a 1-bit left rotation
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    
    return acc;
}