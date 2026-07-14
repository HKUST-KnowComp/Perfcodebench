#include "interface.h"
#include "xxhash.h"
#include <string>
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const char* data = input.data();
    const std::size_t size = input.size();
    std::size_t offset = 0;

    while (offset < size) {
        const std::size_t remaining = size - offset;
        const std::size_t take = (remaining < kChunkSize) ? remaining : kChunkSize;
        
        // Use the pointer directly to avoid std::string::substr allocations
        uint64_t hash = XXH3_64bits(data + offset, take);
        
        acc ^= hash;
        acc = (acc << 1) | (acc >> 63);
        
        offset += take;
    }

    return acc;
}