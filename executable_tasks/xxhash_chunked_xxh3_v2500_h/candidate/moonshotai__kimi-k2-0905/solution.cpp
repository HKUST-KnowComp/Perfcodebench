#include "interface.h"
#include "xxhash.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const char* data = input.data();
    const std::size_t size = input.size();
    std::size_t offset = 0;

    while (offset + kChunkSize <= size) {
        uint64_t h = XXH3_64bits(data + offset, kChunkSize);
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
        offset += kChunkSize;
    }

    std::size_t tail = size - offset;
    if (tail) {
        uint64_t h = XXH3_64bits(data + offset, tail);
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }

    return acc;
}