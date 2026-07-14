#include "interface.h"

#include "xxhash.h"

#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    const char* data = input.data();
    const std::size_t total = input.size();
    uint64_t acc = 0;
    std::size_t offset = 0;
    // Process full chunks
    for (; offset + kChunkSize <= total; offset += kChunkSize) {
        acc ^= static_cast<uint64_t>(XXH64(data + offset, kChunkSize, 0));
        acc = (acc << 1) | (acc >> 63);
    }
    // Process remaining bytes (if any)
    if (offset < total) {
        acc ^= static_cast<uint64_t>(XXH64(data + offset, total - offset, 0));
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}
