#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const std::size_t size = input.size();
    const char* data = input.data();
    std::size_t offset = 0;
    const std::size_t full_chunks = size / kChunkSize;
    for (std::size_t i = 0; i < full_chunks; ++i) {
        acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, kChunkSize));
        acc = (acc << 1) | (acc >> 63);
        offset += kChunkSize;
    }
    if (offset < size) {
        const std::size_t remainder = size - offset;
        acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, remainder));
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}
