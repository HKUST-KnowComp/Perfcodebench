#include "interface.h"
#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    std::size_t size = input.size();
    std::size_t start = 0;
    while (start < size) {
        const char* nl = static_cast<const char*>(std::memchr(data + start, '\n', size - start));
        std::size_t length = nl ? (nl - (data + start)) : (size - start);
        acc ^= static_cast<uint64_t>(XXH64(data + start, length, 0));
        acc *= 1099511628211ULL;
        start = nl ? (nl - data + 1) : size;
    }
    return acc;
}