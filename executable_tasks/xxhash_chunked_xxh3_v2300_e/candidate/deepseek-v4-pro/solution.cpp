#include "interface.h"

#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    const char* data = input.data();
    const std::size_t size = input.size();
    uint64_t acc = 0;
    std::size_t offset = 0;
    for (; offset + kChunkSize <= size; offset += kChunkSize) {
        uint64_t h = static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    if (offset < size) {
        uint64_t h = static_cast<uint64_t>(XXH3_64bits(data + offset, size - offset));
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}