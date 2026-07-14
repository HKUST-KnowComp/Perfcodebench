#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <string>
#include <cstdint>

/**
 * Optimized chunked XXH3 implementation.
 * 
 * The baseline implementation was slow primarily due to `input.substr(offset, take)`,
 * which allocates a new string and copies data for every 1KB chunk.
 * 
 * This version:
 * 1. Eliminates all memory allocations and copies by using raw pointers into the input string.
 * 2. Uses the high-performance XXH3_64bits function directly on the source memory.
 * 3. Maintains the exact bitwise logic (XOR followed by a 1-bit left rotation) required by the task.
 */
uint64_t chunked_hash(const std::string& input) {
    const size_t total_size = input.size();
    const char* const data_ptr = input.data();
    
    uint64_t acc = 0;
    constexpr size_t kChunkSize = 1024;

    size_t offset = 0;
    // Process full chunks
    while (offset + kChunkSize <= total_size) {
        uint64_t h = XXH3_64bits(data_ptr + offset, kChunkSize);
        acc ^= h;
        // Circular left shift by 1
        acc = (acc << 1) | (acc >> 63);
        offset += kChunkSize;
    }

    // Process the remaining partial chunk, if any
    if (offset < total_size) {
        const size_t remaining = total_size - offset;
        uint64_t h = XXH3_64bits(data_ptr + offset, remaining);
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }

    return acc;
}