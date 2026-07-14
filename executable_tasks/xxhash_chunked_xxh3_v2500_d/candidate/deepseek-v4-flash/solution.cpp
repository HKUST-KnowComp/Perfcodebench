#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t chunked_hash(const std::string& input) {
    constexpr std::size_t kChunkSize = 1024;
    const std::size_t input_size = input.size();
    uint64_t acc = 0;
    for (std::size_t offset = 0; offset < input_size; offset += kChunkSize) {
        const std::size_t take = (offset + kChunkSize > input_size) ? (input_size - offset) : kChunkSize;
        acc ^= static_cast<uint64_t>(XXH3_64bits(input.data() + offset, take));
        acc = (acc << 1) | (acc >> 63);
    }
    return acc;
}