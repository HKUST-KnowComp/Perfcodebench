#include "interface.h"
#include "xxhash.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

// Left rotate a 64-bit value by 1 bit.
inline uint64_t rotl1(uint64_t x) noexcept {
    return (x << 1) | (x >> 63);
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const std::size_t len = input.size();
    const char* data = input.data();
    for (std::size_t offset = 0; offset < len; offset += kChunkSize) {
        const std::size_t take = (offset + kChunkSize <= len) ? kChunkSize : (len - offset);
        acc ^= XXH3_64bits(data + offset, take);
        acc = rotl1(acc);
    }
    return acc;
}