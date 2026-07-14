#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <string>
#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1024;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const char* const data = input.data();
    const std::size_t total_size = input.size();

    for (std::size_t offset = 0; offset < total_size; offset += kChunkSize) {
        // Calculate the size of the current chunk without creating a substring object
        const std::size_t take = (total_size - offset < kChunkSize) ? (total_size - offset) : kChunkSize;
        
        // Compute XXH3_64bits directly on the memory segment
        const uint64_t chunk_hash = XXH3_64bits(data + offset, take);
        
        // Accumulate using XOR and a 1-bit left circular shift (rotate)
        acc ^= chunk_hash;
        acc = (acc << 1) | (acc >> 63);
    }
    
    return acc;
}