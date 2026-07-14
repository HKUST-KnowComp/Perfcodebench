#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    const char* data = input.data();
    const std::size_t size = input.size();
    uint64_t acc = 0;
    const std::size_t num_full_chunks = size / kChunkSize;
    for (std::size_t i = 0; i < num_full_chunks; ++i) {
        const uint64_t h = static_cast<uint64_t>(XXH3_64bits(data + i * kChunkSize, kChunkSize));
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    const std::size_t remainder = size % kChunkSize;
    if (remainder) {
        const uint64_t h = static_cast<uint64_t>(XXH3_64bits(data + num_full_chunks * kChunkSize, remainder));
        acc ^= h;
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}
