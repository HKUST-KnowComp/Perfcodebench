#include "interface.h"
#include "xxhash.h"

uint64_t chunked_hash(const std::string& input) {
    constexpr std::size_t kChunkSize = 1 << 10;
    const char* data = input.data();
    const std::size_t size = input.size();
    uint64_t acc = 0;
    const std::size_t fullChunks = size >> 10; // size / 1024
    const std::size_t remainder = size & (kChunkSize - 1); // size % 1024
    const char* ptr = data;
    for (std::size_t i = 0; i < fullChunks; ++i) {
        acc ^= static_cast<uint64_t>(XXH3_64bits(ptr, kChunkSize));
        acc = (acc << 1) | (acc >> 63);
        ptr += kChunkSize;
    }
    if (remainder) {
        acc ^= static_cast<uint64_t>(XXH3_64bits(ptr, remainder));
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}