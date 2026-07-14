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
        const std::size_t take = (total_size - offset < kChunkSize) ? (total_size - offset) : kChunkSize;
        
        // Directly call XXH3_64bits on the memory segment to avoid std::string allocations/copies
        const uint64_t h = XXH3_64bits(data + offset, take);
        
        acc ^= h;
        // Circular left shift by 1 bit
        acc = (acc << 1) | (acc >> 63);
    }
    
    return acc;
}