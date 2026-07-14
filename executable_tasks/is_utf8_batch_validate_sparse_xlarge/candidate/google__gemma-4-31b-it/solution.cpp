#include "interface.h"
#include <cstdint>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    auto valid_scalar = [](const std::string& s) -> bool {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(s.data());
        const uint8_t* end = p + s.size();

        while (p < end) {
            // Fast path: process 8 bytes at a time to find non-ASCII characters
            if (end - p >= 8) {
                uint64_t chunk;
                // Use a safe copy to avoid alignment issues on some architectures
                __builtin_memcpy(&chunk, p, 8);
                if ((chunk & 0x8080808080808080ULL) == 0) {
                    p += 8;
                    continue;
                }
            }

            const uint8_t c = *p++;
            if (c <= 0x7F) continue;

            // 2-byte sequence
            if (c >= 0xC2 && c <= 0xDF) {
                if (p >= end || (*p++ & 0xC0) != 0x80) return false;
                continue;
            }

            // 3-byte sequence
            if (c >= 0xE0 && c <= 0xEF) {
                if (end - p < 2) return false;
                uint8_t c1 = p[0];
                uint8_t c2 = p[1];
                if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80) return false;
                if (c == 0xE0 && c1 < 0xA0) return false;
                if (c == 0xED && c1 > 0x9F) return false;
                p += 2;
                continue;
            }

            // 4-byte sequence
            if (c >= 0xF0 && c <= 0xF4) {
                if (end - p < 3) return false;
                uint8_t c1 = p[0];
                uint8_t c2 = p[1];
                uint8_t c3 = p[2];
                if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80) return false;
                if (c == 0xF0 && c1 < 0x90) return false;
                if (c == 0xF4 && c1 > 0x8F) return false;
                p += 3;
                continue;
            }

            return false;
        }
        return true;
    };

    std::size_t count = 0;
    for (const auto& s : inputs) {
        if (valid_scalar(s)) {
            count++;
        }
    }
    return count;
}