#include "interface.h"

#include "xxhash.h"

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const char* data = input.data();
    const std::size_t size = input.size();
    for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
        const std::size_t chunk_size = (offset + kChunkSize <= size) ? kChunkSize : (size - offset);
        acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, chunk_size));
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}