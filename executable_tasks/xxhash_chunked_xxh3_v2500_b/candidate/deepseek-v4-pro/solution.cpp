#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const std::size_t len = input.size();
    const std::size_t full_chunks = len / kChunkSize;
    const std::size_t rem = len % kChunkSize;
    const char* data = input.data();

    for (std::size_t i = 0; i < full_chunks; ++i) {
        const char* chunk_ptr = data + i * kChunkSize;
        acc ^= static_cast<uint64_t>(XXH3_64bits(chunk_ptr, kChunkSize));
        acc = (acc << 1) | (acc >> 63);
    }
    if (rem > 0) {
        const char* chunk_ptr = data + full_chunks * kChunkSize;
        acc ^= static_cast<uint64_t>(XXH3_64bits(chunk_ptr, rem));
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}