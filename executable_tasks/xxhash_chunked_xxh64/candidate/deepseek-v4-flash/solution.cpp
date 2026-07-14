#include "interface.h"
#include "xxhash.h"
#include <cstddef>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const char* data = input.data();
    std::size_t size = input.size();
    for (std::size_t offset = 0; offset < size; offset += kChunkSize) {
        std::size_t take = (offset + kChunkSize > size) ? (size - offset) : kChunkSize;
        acc ^= static_cast<uint64_t>(XXH64(data + offset, take, 0));
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}
