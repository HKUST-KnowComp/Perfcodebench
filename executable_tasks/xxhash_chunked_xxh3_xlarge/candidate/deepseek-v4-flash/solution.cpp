#include "interface.h"
#include "xxhash.h"
#include <cstddef>

uint64_t chunked_hash(const std::string& input) {
    constexpr std::size_t kChunkSize = 1024;
    uint64_t acc = 0;
    const char* data = input.data();
    std::size_t len = input.size();
    for (std::size_t offset = 0; offset < len; offset += kChunkSize) {
        std::size_t chunk_len = kChunkSize;
        if (offset + chunk_len > len) {
            chunk_len = len - offset;
        }
        acc ^= static_cast<uint64_t>(XXH3_64bits(data + offset, chunk_len));
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}