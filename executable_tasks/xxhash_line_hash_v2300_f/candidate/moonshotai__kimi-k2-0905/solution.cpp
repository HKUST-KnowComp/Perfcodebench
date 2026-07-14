#include "interface.h"
#include "xxhash.h"
#include <string>

uint64_t hash_lines(const std::string& input) {
    const char* data = input.data();
    const std::size_t n = input.size();
    uint64_t acc = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    std::size_t start = 0;
    while (start < n) {
        std::size_t end = start;
        while (end < n && data[end] != '\n') ++end;
        acc ^= XXH64(data + start, end - start, 0);
        acc *= prime;
        start = end + 1;
    }
    return acc;
}