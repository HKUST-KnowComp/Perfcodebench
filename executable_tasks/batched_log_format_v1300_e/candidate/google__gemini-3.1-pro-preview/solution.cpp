#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
    
    if (iters <= 0) {
        return 0;
    }

    uint64_t hash = 1469598103934665603ULL;
    size_t n = ids.size();
    
    for (size_t i = 0; i < n; ++i) {
        uint32_t val = timestamps[i];
        char buf[32];
        char* p = buf + 32;
        do {
            *--p = '0' + (val % 10);
            val /= 10;
        } while (val > 0);
        while (p < buf + 32) {
            hash ^= static_cast<unsigned char>(*p++);
            hash *= 1099511628211ULL;
        }

        hash ^= '|';
        hash *= 1099511628211ULL;

        val = ids[i];
        p = buf + 32;
        do {
            *--p = '0' + (val % 10);
            val /= 10;
        } while (val > 0);
        while (p < buf + 32) {
            hash ^= static_cast<unsigned char>(*p++);
            hash *= 1099511628211ULL;
        }

        hash ^= '|';
        hash *= 1099511628211ULL;

        uint16_t val16 = levels[i];
        p = buf + 32;
        do {
            *--p = '0' + (val16 % 10);
            val16 /= 10;
        } while (val16 > 0);
        while (p < buf + 32) {
            hash ^= static_cast<unsigned char>(*p++);
            hash *= 1099511628211ULL;
        }

        hash ^= '|';
        hash *= 1099511628211ULL;

        for (unsigned char c : messages[i]) {
            hash ^= c;
            hash *= 1099511628211ULL;
        }

        hash ^= '\n';
        hash *= 1099511628211ULL;
    }

    return hash;
}
