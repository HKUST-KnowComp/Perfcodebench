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
    const std::size_t size = input.size();
    
    std::size_t offset = 0;
    // Process full chunks
    if (size >= kChunkSize) {
        const std::size_t end_offset = size - kChunkSize;
        for (; offset <= end_offset; offset += kChunkSize) {
            const uint64_t h = XXH3_64bits(data + offset, kChunkSize);
            acc ^= h;
            acc = (acc << 1) | (acc >> 63);
        }
    }
    
    // Process remaining partial chunk
    if (offset < size) {
        const std::size_t take = size - offset;
        const uint64_t h = XXH3_64bits(data + offset, take);
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    
    return acc;
}