#include "interface.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace {

bool is_valid_utf8(const std::string& s) {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(s.data());
    const uint8_t* end = p + s.size();
    while (p < end) {
        uint8_t c = *p++;
        if (c < 0x80) continue;
        
        uint8_t len = 0;
        if (c < 0xC0) return false;
        if (c < 0xE0) len = 2;
        else if (c < 0xF0) len = 3;
        else if (c < 0xF8) len = 4;
        else return false;

        if (end - p < len - 1) return false;

        uint8_t b1 = p[0];
        if ((b1 & 0xC0) != 0x80) return false;
        if (len > 2) {
            uint8_t b2 = p[1];
            if ((b2 & 0xC0) != 0x80) return false;
            if (len > 3) {
                uint8_t b3 = p[2];
                if ((b3 & 0xC0) != 0x80) return false;
            }
        }

        if (len == 2) {
            if (c < 0xC2) return false;
        } else if (len == 3) {
            if (c == 0xE0 && b1 < 0xA0) return false;
            if (c == 0xED && b1 > 0x9F) return false;
        } else if (len == 4) {
            if (c == 0xF0 && b1 < 0x90) return false;
            if (c == 0xF4 && b1 > 0x8F) return false;
            if (c > 0xF4) return false;
        }
        p += len - 1;
    }
    return true;
}

} // namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        count += is_valid_utf8(s) ? 1U : 0U;
    }
    return count;
}
