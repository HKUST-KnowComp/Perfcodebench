#include "interface.h"
#include "xxhash.h"
#include <algorithm>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    const char* data = input.data();
    const std::size_t size = input.size();
    uint64_t acc = 0;
    
    for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
        const std::size_t take = (size - offset < kChunkSize) ? (size - offset) : kChunkSize;
        uint32_t hash = XXH32(data + offset, take, 0);
        acc ^= static_cast<uint64_t>(hash);
        // Rotate left 1 bit
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}