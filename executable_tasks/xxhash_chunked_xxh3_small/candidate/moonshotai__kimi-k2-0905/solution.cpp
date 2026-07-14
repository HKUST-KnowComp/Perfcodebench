#include "interface.h"
#include "xxhash.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const char* data = input.data();
    const std::size_t len = input.size();
    std::size_t offset = 0;

    for (; offset + kChunkSize <= len; offset += kChunkSize) {
        uint64_t h = XXH3_64bits(data + offset, kChunkSize);
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    if (std::size_t rem = len - offset) {
        uint64_t h = XXH3_64bits(data + offset, rem);
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}