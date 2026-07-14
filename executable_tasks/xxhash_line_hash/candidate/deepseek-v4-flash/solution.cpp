#include "interface.h"
#include "xxhash.h"
#include <string>
#include <cstddef>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    std::size_t len = input.size();
    std::size_t start = 0;
    while (start < len) {
        std::size_t end = start;
        while (end < len && data[end] != '\n') {
            ++end;
        }
        std::size_t seg_len = end - start;
        acc ^= static_cast<uint64_t>(XXH64(data + start, seg_len, 0));
        acc *= 1099511628211ULL;
        start = end + 1;
    }
    return acc;
}