#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10; // 1024
}

uint64_t chunked_hash(const std::string& input) {
    const char* data = input.data();
    std::size_t size = input.size();
    uint64_t acc = 0;
    std::size_t offset = 0;

    // Process full chunks
    for (; offset + kChunkSize <= size; offset += kChunkSize) {
        acc ^= XXH3_64bits(data + offset, kChunkSize);
        acc = (acc << 1) | (acc >> 63);
    }

    // Process final partial chunk
    if (offset < size) {
        acc ^= XXH3_64bits(data + offset, size - offset);
        acc = (acc << 1) | (acc >> 63);
    }

    return acc;
}
