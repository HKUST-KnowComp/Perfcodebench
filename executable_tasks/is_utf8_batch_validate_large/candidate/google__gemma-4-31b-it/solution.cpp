#include "interface.h"
#include <cstdint>

static inline bool is_valid_utf8(const std::string& s) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
    const unsigned char* end = p + s.size();

    while (p < end) {
        // Fast path: process 8 bytes at a time if they are all ASCII
        while (p + 8 <= end) {
            uint64_t chunk;
            __builtin_memcpy(&chunk, p, 8);
            if ((chunk & 0x8080808080808080ULL) == 0) {
                p += 8;
            } else {
                break;
            }
        }

        if (p >= end) break;

        const unsigned char c = *p++;
        if (c <= 0x7F) continue;

        if (c >= 0xC2 && c <= 0xDF) {
            if (p >= end || (p[0] & 0xC0) != 0x80) return false;
            p++;
        } else if (c >= 0xE0 && c <= 0xEF) {
            if (end - p < 2) return false;
            unsigned char c2 = p[0];
            unsigned char c3 = p[1];
            if ((c3 & 0xC0) != 0x80) return false;
            if (c == 0xE0) {
                if (c2 < 0xA0 || c2 > 0xBF) return false;
            } else if (c == 0xED) {
                if (c2 < 0x80 || c2 > 0x9F) return false;
            } else {
                if ((c2 & 0xC0) != 0x80) return false;
            }
            p += 2;
        } else if (c >= 0xF0 && c <= 0xF4) {
            if (end - p < 3) return false;
            unsigned char c2 = p[0];
            unsigned char c3 = p[1];
            unsigned char c4 = p[2];
            if ((c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80) return false;
            if (c == 0xF0) {
                if (c2 < 0x90 || c2 > 0xBF) return false;
            } else if (c == 0xF4) {
                if (c2 < 0x80 || c2 > 0x8F) return false;
            } else {
                if ((c2 & 0xC0) != 0x80) return false;
            }
            p += 3;
        } else {
            return false;
        }
    }
    return true;
}

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        if (is_valid_utf8(s)) {
            count++;
        }
    }
    return count;
}