#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>

uint64_t chunked_hash(const std::string& input) {
    const uint64_t h = XXH64(input.data(), input.size(), 0);
    uint64_t acc = h ^ (h >> 32);
    acc = (acc << 1) | (acc >> 63);
    return acc;
}