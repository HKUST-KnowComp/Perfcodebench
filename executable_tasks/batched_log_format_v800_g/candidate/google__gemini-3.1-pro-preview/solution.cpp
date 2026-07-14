#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

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
    const uint64_t prime = 1099511628211ULL;

    auto add_char = [&](char c) {
        hash ^= static_cast<uint8_t>(c);
        hash *= prime;
    };

    auto add_str = [&](const std::string& s) {
        for (char c : s) {
            hash ^= static_cast<uint8_t>(c);
            hash *= prime;
        }
    };

    auto add_num = [&](uint32_t v) {
        char buf[10];
        char* p = buf + 10;
        do {
            uint32_t q = v / 10;
            *--p = '0' + static_cast<char>(v - q * 10);
            v = q;
        } while (v > 0);
        while (p < buf + 10) {
            hash ^= static_cast<uint8_t>(*p++);
            hash *= prime;
        }
    };

    std::size_t n = ids.size();
    for (std::size_t i = 0; i < n; ++i) {
        add_num(timestamps[i]);
        add_char('|');
        add_num(ids[i]);
        add_char('|');
        add_num(levels[i]);
        add_char('|');
        add_str(messages[i]);
        add_char('\n');
    }

    return hash;
}
