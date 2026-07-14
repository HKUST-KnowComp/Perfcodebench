#include "interface.h"
#include "xxhash.h"
#include <cstdint>

uint64_t chunked_hash(const std::string& input) {
    const std::size_t size = input.size();
    const char* data = input.data();
    constexpr std::size_t kChunkSize = 1 << 10;
    uint64_t acc = 0;
    std::size_t offset = 0;

    // Process full chunks
    for (; offset + kChunkSize <= size; offset += kChunkSize) {
        const uint64_t h = XXH64(data + offset, kChunkSize, 0);
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }

    // Process remaining bytes (if any)
    if (offset < size) {
        const uint64_t h = XXH64(data + offset, size - offset, 0);
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }

    return acc;
}