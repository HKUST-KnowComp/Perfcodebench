#include "interface.h"
#include <cstdint>
#include <cstring>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    static constexpr uint8_t utf8_len[256] = {
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0
    };

    std::size_t count = 0;
    for (const auto& s : inputs) {
        const uint8_t* p = reinterpret_cast<const uint8_t*>(s.data());
        const uint8_t* end = p + s.size();
        bool valid = true;

        // Fast path: skip 8 ASCII bytes at once
        while (p + 8 <= end) {
            uint64_t w;
            std::memcpy(&w, p, 8);
            if (w & 0x8080808080808080ULL) break;
            p += 8;
        }

        while (p < end) {
            uint8_t c = *p++;
            uint8_t len = utf8_len[c];
            if (len == 1) continue;
            if (len == 0) { valid = false; break; }
            if (p + len - 1 > end) { valid = false; break; }

            if ((p[0] & 0xC0) != 0x80 || (len > 2 && (p[1] & 0xC0) != 0x80) || (len > 3 && (p[2] & 0xC0) != 0x80)) {
                valid = false; break;
            }

            if (c == 0xE0 && p[0] < 0xA0) { valid = false; break; }
            if (c == 0xED && p[0] > 0x9F) { valid = false; break; }
            if (c == 0xF0 && p[0] < 0x90) { valid = false; break; }
            if (c == 0xF4 && p[0] > 0x8F) { valid = false; break; }

            p += len - 1;
        }
        if (valid) ++count;
    }
    return count;
}