#include "interface.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
        const unsigned char* end = p + s.size();
        bool valid = true;
        while (p < end) {
            while (p < end && *p < 0x80) ++p;
            if (p == end) break;

            unsigned char c = *p;
            int len;
            if (c < 0xE0) len = 2;
            else if (c < 0xF0) len = 3;
            else if (c < 0xF5) len = 4;
            else { valid = false; break; }

            if (end - p < len) { valid = false; break; }

            if ((p[1] & 0xC0) != 0x80) { valid = false; break; }
            if (len > 2 && (p[2] & 0xC0) != 0x80) { valid = false; break; }
            if (len > 3 && (p[3] & 0xC0) != 0x80) { valid = false; break; }

            if (len == 2) {
                if (c < 0xC2) { valid = false; break; }
            } else if (len == 3) {
                if (c == 0xE0 && p[1] < 0xA0) { valid = false; break; }
                if (c == 0xED && p[1] > 0x9F) { valid = false; break; }
            } else {
                if (c == 0xF0 && p[1] < 0x90) { valid = false; break; }
                if (c == 0xF4 && p[1] > 0x8F) { valid = false; break; }
            }
            p += len;
        }
        if (valid) ++count;
    }
    return count;
}