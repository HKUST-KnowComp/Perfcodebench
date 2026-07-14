#include "interface.h"
#include <cstddef>
#include <string>
#include <vector>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
        const unsigned char* end = p + s.size();
        bool valid = true;
        while (p < end) {
            unsigned char c = *p;
            if (c < 0x80) {
                p++;
                continue;
            }
            if (c < 0xC2 || c > 0xF4) {
                valid = false;
                break;
            }
            unsigned int len = (c < 0xE0) ? 2 : (c < 0xF0) ? 3 : 4;
            if (end - p < len) {
                valid = false;
                break;
            }
            unsigned char c1 = p[1];
            if ((c1 & 0xC0) != 0x80) { valid = false; break; }
            if (len > 2) {
                unsigned char c2 = p[2];
                if ((c2 & 0xC0) != 0x80) { valid = false; break; }
                if (len > 3) {
                    unsigned char c3 = p[3];
                    if ((c3 & 0xC0) != 0x80) { valid = false; break; }
                }
            }
            if (c == 0xE0 && c1 < 0xA0) { valid = false; break; }
            if (c == 0xED && c1 > 0x9F) { valid = false; break; }
            if (c == 0xF0 && c1 < 0x90) { valid = false; break; }
            if (c == 0xF4 && c1 > 0x8F) { valid = false; break; }
            p += len;
        }
        if (valid) count++;
    }
    return count;
}