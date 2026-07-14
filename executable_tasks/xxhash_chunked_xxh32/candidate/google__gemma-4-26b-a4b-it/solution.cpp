#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1024;
}

uint64_t chunked_hash(const std::string& input) {
    const char* data = input.data();
    const std::size_t size = input.size();
    uint64_t acc = 0;
    std::size_t offset = 0;

    // Process full chunks
    const std::size_t full_chunks_end = (size >= kChunkSize) ? (size - kChunkSize + 1) : 0;
    
    // We use a manual loop to avoid the overhead of std::min and repeated subtractions
    // The loop condition ensures we only process complete kChunkSize blocks
    while (offset + kChunkSize <= size) {
        acc ^= static_cast<uint64_t>(XXH32(data + offset, kChunkSize, 0));
        acc = (acc << 1) | (acc >> 63);
        offset += kChunkSize;
    }

    // Process the remaining partial chunk if it exists
    if (offset < size) {
        acc ^= static_cast<uint64_t>(XXH32(data + offset, size - offset, 0));
        acc = (acc << 1) | (acc >> 63);
    }

    return acc;
}