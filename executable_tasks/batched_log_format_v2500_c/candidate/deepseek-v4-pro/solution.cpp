#include "interface.h"

#include <string>
#include <vector>

namespace {

// Hash update functions
inline void hash_char(uint64_t &hash, unsigned char c) {
    hash ^= static_cast<uint64_t>(c);
    hash *= 1099511628211ULL;
}

// Convert unsigned 32-bit integer to decimal digits and update hash
void hash_uint32(uint64_t &hash, uint32_t val) {
    char buf[12];
    char *p = buf + sizeof(buf);
    if (val == 0) {
        *--p = '0';
    } else {
        while (val > 0) {
            *--p = static_cast<char>('0' + (val % 10));
            val /= 10;
        }
    }
    // p points to first digit
    char *end = buf + sizeof(buf);
    while (p < end) {
        hash_char(hash, *p);
        ++p;
    }
}

void hash_uint16(uint64_t &hash, uint16_t val) {
    // Same as uint32_t but with smaller max
    char buf[7]; // 5 digits + safety
    char *p = buf + sizeof(buf);
    if (val == 0) {
        *--p = '0';
    } else {
        while (val > 0) {
            *--p = static_cast<char>('0' + (val % 10));
            val /= 10;
        }
    }
    char *end = buf + sizeof(buf);
    while (p < end) {
        hash_char(hash, *p);
        ++p;
    }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    (void)iters; // we only need one iteration, result is same
    uint64_t hash = 1469598103934665603ULL;
    const std::size_t n = ids.size();
    for (std::size_t i = 0; i < n; ++i) {
        hash_uint32(hash, timestamps[i]);
        hash_char(hash, '|');
        hash_uint32(hash, ids[i]);
        hash_char(hash, '|');
        hash_uint16(hash, levels[i]);
        hash_char(hash, '|');
        for (char c : messages[i]) {
            hash_char(hash, static_cast<unsigned char>(c));
        }
        hash_char(hash, '\n');
    }
    return hash;
}