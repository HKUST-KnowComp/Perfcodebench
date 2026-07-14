#include "interface.h"

#include <cstdint>
#include <string>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
    if (iters <= 0) {
        return 0;
    }
    uint64_t hash = 1469598103934665603ULL;
    const char* data = text.data();
    size_t n = text.size();
    size_t i = 0;
    while (i < n) {
        size_t j = i;
        while (j < n && data[j] != '\n') ++j;
        size_t len = j - i;
        hash ^= len;
        hash *= 1099511628211ULL;
        hash ^= (len == 0) ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
        i = j + 1;
    }
    return hash;
}
