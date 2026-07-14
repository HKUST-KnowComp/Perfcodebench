#include "interface.h"
#include "xxhash.h"
#include <string>
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const size_t n = input.size();
    const char* data = input.data();

    for (size_t offset = 0; offset < n; offset += kChunkSize) {
        size_t remaining = n - offset;
        size_t take = (remaining < kChunkSize) ? remaining : kChunkSize;
        
        // Use the pointer directly to avoid std::string substr() allocations/copies
        uint64_t hash = XXH3_64bits(data + offset, take);
        
        acc ^= hash;
        // Circular left shift (rotate) by 1
        acc = (acc << 1) | (acc >> 63);
    }
    
    return acc;
}