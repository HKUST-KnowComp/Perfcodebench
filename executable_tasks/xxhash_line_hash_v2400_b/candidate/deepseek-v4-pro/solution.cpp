#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    const std::size_t size = input.size();
    std::size_t start = 0;
    while (start < size) {
        std::size_t end = start;
        while (end < size && data[end] != '\n') {
            ++end;
        }
        const std::size_t len = end - start;
        acc ^= static_cast<uint64_t>(XXH64(data + start, len, 0));
        acc *= 1099511628211ULL;
        start = end + 1;
    }
    return acc;
}
