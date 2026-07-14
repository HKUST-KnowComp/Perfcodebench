#include "interface.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    auto is_valid_utf8 = [](const std::string& s) -> bool {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
        const unsigned char* end = p + s.size();

        while (p < end) {
            unsigned char c = *p++;
            if (c <= 0x7F) continue;

            if ((c & 0xE0) == 0xC0) {
                if (p >= end || (c < 0xC2) || (*p++ & 0xC0) != 0x80) return false;
            } else if ((c & 0xF0) == 0xE0) {
                if (p + 1 >= end) return false;
                unsigned char c2 = *p++;
                unsigned char c3 = *p++;
                if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80) return false;
                if (c == 0xE0 && c2 < 0xA0) return false;
                if (c == 0xED && c2 > 0x9F) return false;
            } else if ((c & 0xF8) == 0xF0) {
                if (p + 2 >= end) return false;
                unsigned char c2 = *p++;
                unsigned char c3 = *p++;
                unsigned char c4 = *p++;
                if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80) return false;
                if (c == 0xF0 && c2 < 0x90) return false;
                if (c == 0xF4 && c2 > 0x8F) return false;
                if (c > 0xF4) return false;
            } else {
                return false;
            }
        }
        return true;
    };

    std::size_t count = 0;
    for (const auto& s : inputs) {
        if (is_valid_utf8(s)) {
            count++;
        }
    }
    return count;
}