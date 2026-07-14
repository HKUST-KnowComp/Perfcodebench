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
        
        // Use the raw pointer to avoid std::string::substr allocations
        acc ^= XXH3_64bits(data + offset, take);
        
        // Circular left shift (rotate left by 1)
        acc = (acc << 1) | (acc >> 63);
        
        offset += take;
    }

    return acc;
}