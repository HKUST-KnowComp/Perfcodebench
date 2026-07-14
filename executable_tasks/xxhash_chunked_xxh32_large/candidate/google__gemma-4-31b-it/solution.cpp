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
    std::size_t offset = 0;

    while (offset < size) {
        std::size_t take = kChunkSize;
        if (offset + take > size) {
            take = size - offset;
        }
        
        acc ^= static_cast<uint64_t>(XXH32(data + offset, take, 0));
        acc = (acc << 1) | (acc >> 63);
        offset += kChunkSize;
    }
    return acc;
}