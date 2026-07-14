#include "interface.h"
#include "xxhash.h"
#include <algorithm>
#include <string>

namespace {
constexpr std::size_t kChunkSize = 1 << 10;
}

uint64_t chunked_hash(const std::string& input) {
    uint64_t acc = 0;
    const size_t input_len = input.size();
    const char* const input_buf = input.data();
    for (size_t offset = 0; offset < input_len; offset += kChunkSize) {
        const size_t remaining = input_len - offset;
        const size_t take = std::min(kChunkSize, remaining);
        const uint64_t chunk_hash = static_cast<uint64_t>(XXH3_64bits(input_buf + offset, take));
        acc ^= chunk_hash;
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}