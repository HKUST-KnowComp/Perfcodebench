#include "interface.h"
#include "xxhash.h"

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    std::size_t pos = 0;
    const std::size_t len = input.size();

    while (pos < len) {
        std::size_t next = input.find('\n', pos);
        if (next == std::string::npos) next = len;
        const std::size_t line_len = next - pos;
        acc ^= static_cast<uint64_t>(XXH64(data + pos, line_len, 0));
        acc *= 1099511628211ULL;
        pos = next + 1;
    }
    return acc;
}