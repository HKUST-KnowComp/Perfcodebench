#include "interface.h"
#include "xxhash.h"

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const size_t n = input.size();
    size_t start = 0;
    while (true) {
        size_t pos = input.find('\n', start);
        if (pos == std::string::npos) {
            if (start < n) {
                acc ^= static_cast<uint64_t>(XXH64(input.data() + start, n - start, 0));
                acc *= 1099511628211ULL;
            }
            break;
        }
        acc ^= static_cast<uint64_t>(XXH64(input.data() + start, pos - start, 0));
        acc *= 1099511628211ULL;
        start = pos + 1;
    }
    return acc;
}