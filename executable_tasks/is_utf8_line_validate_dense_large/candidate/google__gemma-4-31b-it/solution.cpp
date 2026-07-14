#include "interface.h"
#include <cstdint>

std::size_t count_valid_lines(const std::string& input) {
    const unsigned char* p_start = reinterpret_cast<const unsigned char*>(input.data());
    const unsigned char* p_end = p_start + input.size();
    std::size_t valid_count = 0;

    while (p_start < p_end) {
        const unsigned char* line_start = p_start;
        const unsigned char* line_end = p_start;

        // Fast scan for newline
        while (line_end < p_end && *line_end != '\n') {
            line_end++;
        }

        // Validate UTF-8 for the current line
        bool is_valid = true;
        const unsigned char* curr = line_start;
        const unsigned char* limit = line_end;

        while (curr < limit) {
            // Fast path: process 8 bytes at a time if they are all ASCII
            if (limit - curr >= 8) {
                uint64_t word = *reinterpret_cast<const uint64_t*>(curr);
                if ((word & 0x8080808080808080ULL) == 0) {
                    curr += 8;
                    continue;
                }
            }

            unsigned char c = *curr++;
            if (c <= 0x7F) continue;

            if (c >= 0xC2 && c <= 0xDF) {
                if (curr >= limit || (curr[0] & 0xC0) != 0x80) {
                    is_valid = false; break;
                }
                curr++;
            } else if (c >= 0xE0 && c <= 0xEF) {
                if (limit - curr < 2) {
                    is_valid = false; break;
                }
                unsigned char c2 = curr[0];
                unsigned char c3 = curr[1];
                if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80) {
                    is_valid = false; break;
                }
                if (c == 0xE0 && c2 < 0xA0) { is_valid = false; break; }
                if (c == 0xED && c2 > 0x9F) { is_valid = false; break; }
                curr += 2;
            } else if (c >= 0xF0 && c <= 0xF4) {
                if (limit - curr < 3) {
                    is_valid = false; break;
                }
                unsigned char c2 = curr[0];
                unsigned char c3 = curr[1];
                unsigned char c4 = curr[2];
                if ((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80) {
                    is_valid = false; break;
                }
                if (c == 0xF0 && c2 < 0x90) { is_valid = false; break; }
                if (c == 0xF4 && c2 > 0x8F) { is_valid = false; break; }
                curr += 3;
            } else {
                is_valid = false;
                break;
            }
        }

        if (is_valid) valid_count++;
        p_start = (line_end < p_end) ? line_end + 1 : p_end;
    }

    return valid_count;
}