#include "interface.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    const char* p = input.data();
    const char* const end = p + input.size();
    std::size_t count = 0;

    while (p < end) {
        const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
        if (!line_end) line_end = end;

        const unsigned char* u = reinterpret_cast<const unsigned char*>(p);
        const unsigned char* u_end = reinterpret_cast<const unsigned char*>(line_end);
        bool valid = true;

        while (u < u_end) {
            unsigned char c = *u++;
            if (c <= 0x7F) continue;
            if (c < 0xC2 || c > 0xF4) { valid = false; break; }

            int extra = (c <= 0xDF) ? 1 : ((c <= 0xEF) ? 2 : 3);
            if (u_end - u < extra) { valid = false; break; }

            if ((u[0] & 0xC0) != 0x80) { valid = false; break; }
            if (extra > 1 && (u[1] & 0xC0) != 0x80) { valid = false; break; }
            if (extra > 2 && (u[2] & 0xC0) != 0x80) { valid = false; break; }

            if (c == 0xE0 && u[0] < 0xA0) { valid = false; break; }
            if (c == 0xED && u[0] > 0x9F) { valid = false; break; }
            if (c == 0xF0 && u[0] < 0x90) { valid = false; break; }
            if (c == 0xF4 && u[0] > 0x8F) { valid = false; break; }

            u += extra;
        }

        if (valid) ++count;
        p = (line_end < end) ? line_end + 1 : line_end;
    }
    return count;
}
