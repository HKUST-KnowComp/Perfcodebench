#include "interface.h"
#include <cstdint>
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
    const unsigned char* end = p + input.size();
    std::size_t count = 0;

    auto validate_utf8 = [](const unsigned char* start, const unsigned char* end_ptr) -> bool {
        const unsigned char* curr = start;
        while (curr < end_ptr) {
            unsigned char c = *curr++;
            if (c <= 0x7F) continue;
            if (c >= 0xC2 && c <= 0xDF) {
                if (curr >= end_ptr || (curr[0] & 0xC0) != 0x80) return false;
                curr++;
            } else if (c >= 0xE0 && c <= 0xEF) {
                if (end_ptr - curr < 2) return false;
                unsigned char c2 = curr[0];
                unsigned char c3 = curr[1];
                if ((c3 & 0xC0) != 0x80) return false;
                if (c == 0xE0) { if (c2 < 0xA0 || c2 > 0xBF) return false; }
                else if (c == 0xED) { if (c2 < 0x80 || c2 > 0x9F) return false; }
                else { if ((c2 & 0xC0) != 0x80) return false; }
                curr += 2;
            } else if (c >= 0xF0 && c <= 0xF4) {
                if (end_ptr - curr < 3) return false;
                unsigned char c2 = curr[0];
                unsigned char c3 = curr[1];
                unsigned char c4 = curr[2];
                if ((c3 & 0xC0) != 0x80 || (c4 & 0xC0) != 0x80) return false;
                if (c == 0xF0) { if (c2 < 0x90 || c2 > 0xBF) return false; }
                else if (c == 0xF4) { if (c2 < 0x80 || c2 > 0x8F) return false; }
                else { if ((c2 & 0xC0) != 0x80) return false; }
                curr += 3;
            } else {
                return false;
            }
        }
        return true;
    };

    while (p < end) {
        const unsigned char* line_start = p;
        const unsigned char* line_end = p;

        // Fast scan for non-ASCII or newline
        while (line_end < end) {
            if ((end - line_end) >= 8) {
                uint64_t word;
                std::memcpy(&word, line_end, 8);
                // Check if any byte has the high bit set (non-ASCII) or is a newline (0x0A)
                // High bit: word & 0x8080808080808080
                // Newline: word & 0x0A0A0A0A0A0A0A0A (but we need to be careful about other bytes being 0x0A)
                // A more robust way to find the first byte that is either > 127 or == 10:
                bool has_special = false;
                for (int i = 0; i < 8; ++i) {
                    unsigned char b = (unsigned char)((word >> (i * 8)) & 0xFF);
                    if (b > 0x7F || b == 0x0A) {
                        line_end += i;
                        has_special = true;
                        break;
                    }
                }
                if (has_special) break;
                line_end += 8;
            } else {
                if (*line_end > 0x7F || *line_end == 0x0A) break;
                line_end++;
            }
        }

        if (line_end < end && *line_end == 0x0A) {
            // We found a newline, but we must check if there were any non-ASCII chars before it
            // The fast scan might have stopped at a non-ASCII char before the newline
            // We need to find the actual end of the line
            const unsigned char* actual_end = line_start;
            while (actual_end < end && *actual_end != 0x0A) {
                actual_end++;
            }
            if (validate_utf8(line_start, actual_end)) count++;
            p = actual_end + 1;
        } else {
            // Reached end of input
            const unsigned char* actual_end = line_start;
            while (actual_end < end && *actual_end != 0x0A) {
                actual_end++;
            }
            if (validate_utf8(line_start, actual_end)) count++;
            p = actual_end + 1;
        }
    }

    return count;
}