#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const std::size_t len = input.size();
    const char* data = input.data();
    for (std::size_t offset = 0; offset < len; offset += kChunkSize) {
        const std::size_t take = std::min(kChunkSize, len - offset);
        uint64_t h = XXH3_64bits(data + offset, take);
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}