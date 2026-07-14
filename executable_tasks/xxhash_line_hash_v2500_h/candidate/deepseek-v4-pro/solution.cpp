#include "interface.h"

#include "xxhash.h"

#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    size_t pos = 0;
    const size_t len = input.size();
    while (pos < len) {
        size_t end = input.find('\n', pos);
        if (end == std::string::npos) {
            end = len;
        }
        acc ^= XXH64(input.data() + pos, end - pos, 0);
        acc *= 1099511628211ULL;
        pos = end + 1;
    }
    return acc;
}
