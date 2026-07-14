#include "interface.h"

#include "xxhash.h"

#include <string>
#include <cstddef>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const uint64_t mul = 1099511628211ULL;
    const char* data = input.data();
    std::size_t len = input.size();
    std::size_t i = 0;
    while (i < len) {
        std::size_t end = i;
        while (end < len && data[end] != '\n') {
            ++end;
        }
        const uint64_t hash = XXH64(data + i, end - i, 0);
        acc ^= hash;
        acc *= mul;
        i = end + 1;
    }
    return acc;
}
