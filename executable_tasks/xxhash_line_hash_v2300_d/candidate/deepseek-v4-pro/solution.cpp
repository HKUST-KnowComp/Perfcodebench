#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
    const char* const data = input.data();
    const std::size_t size = input.size();
    uint64_t acc = 1469598103934665603ULL;
    std::size_t start = 0;
    while (start < size) {
        std::size_t end = input.find('\n', start);
        if (end == std::string::npos) {
            end = size;
        }
        const std::size_t len = end - start;
        acc ^= static_cast<uint64_t>(XXH64(data + start, len, 0));
        acc *= 1099511628211ULL;
        start = end + 1;
    }
    return acc;
}