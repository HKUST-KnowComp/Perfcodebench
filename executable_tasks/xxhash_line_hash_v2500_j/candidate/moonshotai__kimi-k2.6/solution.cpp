#include "interface.h"

#include "xxhash.h"

#include <cstring>
#include <string>

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    const char* const end = data + input.size();
    const char* p = data;

    while (p < end) {
        const char* nl = static_cast<const char*>(std::memchr(p, '\n', static_cast<std::size_t>(end - p)));
        const char* line_end = nl ? nl : end;
        acc ^= static_cast<uint64_t>(XXH64(p, static_cast<std::size_t>(line_end - p), 0));
        acc *= 1099511628211ULL;
        p = nl ? (nl + 1) : end;
    }
    return acc;
}
